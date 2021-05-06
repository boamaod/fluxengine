#include "globals.h"
#include "flags.h"
#include "dataspec.h"
#include "sector.h"
#include "sectorset.h"
#include "imagereader/imagereader.h"
#include "fmt/format.h"
#include "writer.h"
#include "arch/ibm/ibm.h"
#include <algorithm>
#include <iostream>
#include <fstream>

static unsigned int getModulationandSpeed(uint8_t flags, bool *mfm)
{
	switch (flags)
		{
			case 0: /* 500 kbps FM */
				*mfm = false;
				return 500;
				break;

			case 1: /* 300 kbps FM */
				*mfm  = false;
				return 300;
				
				break;

			case 2: /* 250 kbps FM */
				*mfm  = false;
				return 250;
				break;

			case 3: /* 500 kbps MFM */
				*mfm  = true;
				return 500;
				break;

			case 4: /* 300 kbps MFM */
				*mfm  = true;
				return 300;
				break;

			case 5: /* 250 kbps MFM */
				*mfm  = true;
				return 250;
				break;

			default:
				Error() << fmt::format("don't understand IMD disks with this modulation and speed {}", flags);
		}
}

struct TrackHeader
{
	uint8_t ModeValue;
	uint8_t track;
	uint8_t Head;
	uint8_t numSectors;
	uint8_t SectorSize;
};

static unsigned int getSectorSize(uint8_t flags)
{
	switch (flags)
	{
		case 0: return 128;
		case 1: return 256;
		case 2: return 512;
		case 3: return 1024;
		case 4: return 2048;
		case 5: return 4096;
		case 6: return 8192;
	}
	Error() << fmt::format("Sector size {} not in standard range (0-6).", flags);
}

#define SEC_CYL_MAP_FLAG  0x80 
#define SEC_HEAD_MAP_FLAG 0x40
#define HEAD_MASK         0x3F
#define END_OF_FILE       0x1A


class IMDImageReader : public ImageReader
{
public:
	IMDImageReader(const ImageSpec& spec):
		ImageReader(spec)
	{}

	SectorSet readImage()
	/*
	*IMAGE FILE FORMAT
	*The overall layout of an ImageDisk .IMD image file is:
	*IMD v.vv: dd/mm/yyyy hh:mm:ss
	*Comment (ASCII only - unlimited size)
	*1A byte - ASCII EOF character
	*- For each track on the disk:
	*1 byte Mode value							see getModulationspeed for definition		
	*1 byte Cylinder
	*1 byte Head
	*1 byte number of sectors in track			
	*1 byte sector size							see getsectorsize for definition
	*sector numbering map
	*sector cylinder map (optional)				definied in high byte of head (since head is 0 or 1)
	*sector head map (optional)					definied in high byte of head (since head is 0 or 1)
	*sector data records
	*<End of file>
	*/
	{
		//Read File
        std::ifstream inputFile(spec.filename, std::ios::in | std::ios::binary);
        if (!inputFile.is_open())
            Error() << "cannot open input file";
		//define some variables
		int startSectorId = 0;
		bool mfm = false;
		inputFile.seekg(0, inputFile.end);
		int inputFileSize = inputFile.tellg();	// determine filesize
		inputFile.seekg(0, inputFile.beg);
		Bytes data;
		data.writer() += inputFile;
		ByteReader br(data);
		SectorSet sectors;
		TrackHeader header = {0, 0, 0, 0, 0};
		TrackHeader previousheader = {0, 0, 0, 0, 0};

		unsigned int n = 0;
		unsigned int headerPtr = 0;
		unsigned int Modulation_Speed = 0;
		unsigned int sectorSize = 0;
		unsigned int previoussectorSize = 0;
		std::string sector_skew;
		std::string previous_sector_skew;
		previous_sector_skew.clear();	//added because fluxengine cannot handle different sector_skew in an image
		sector_skew.clear();
		uint8_t b; 	
		std::string comment;
		bool blnOptionalCylinderMap = false;
		bool blnOptionalHeadMap = false;

		// Read comment
		comment.clear();
		while ((b = br.read_8()) != EOF && b != END_OF_FILE)
		{
			comment.push_back(b);
			n++;
		}
		headerPtr = n; //set pointer to after comment
		//write comment to screen
		std::cout 	<< "Comment in IMD image:\n"
					<< fmt::format("{}\n\n",
					comment);

		//first read header
		bool blnBaseOne = false; 
		for (;;)
		{
			if (headerPtr >= inputFileSize-1)
			{
				break;
			}
			header.ModeValue = br.read_8();
			headerPtr++;
			Modulation_Speed = getModulationandSpeed(header.ModeValue, &mfm);
			header.track = br.read_8();
			headerPtr++;
			header.Head = br.read_8();
			headerPtr++;
			header.numSectors = br.read_8();
			headerPtr++;
			header.SectorSize = br.read_8();
			headerPtr++;
			sectorSize = getSectorSize(header.SectorSize);

			unsigned int optionalsector_map[header.numSectors];
			//Read optional cylinder map
			//The Sector Cylinder Map has one entry for each sector, and contains the logical Cylinder ID for the corresponding sector in the Sector Numbering Map.
			if (header.Head & SEC_CYL_MAP_FLAG) 
			{
				//Read optional cylinder map
				for (b = 0;  b < header.numSectors; b++)
				{
					optionalsector_map[b] = br.read_8();
					headerPtr++;
				}
				blnOptionalCylinderMap = true;				//set bool so we know there is an optional cylinder map
				header.Head = header.Head^SEC_CYL_MAP_FLAG; //remove flag 10000001 ^ 10000000 = 00000001 and 10000000 ^ 10000000 = 00000000 
			}
			//Read optional sector head map
			//The Sector Head Map has one entry for each sector, and contains the logical Head ID for the corresponding sector in the Sector Numbering Map.
			unsigned int optionalhead_map[header.numSectors];
			if (header.Head & SEC_HEAD_MAP_FLAG) 
			{
				//Read optional sector head map 
				for (b = 0;  b < header.numSectors; b++)
				{
					optionalhead_map[b] = br.read_8();
					headerPtr++;
				}
				blnOptionalHeadMap = true;					//set bool so we know there is an optional head map
				header.Head = header.Head^SEC_HEAD_MAP_FLAG; //remove flag 01000001 ^ 01000001 = 00000001 and 01000000 ^ 0100000 = 00000000 for writing sector head later
			}
			//read sector numbering map
			unsigned int sector_map[header.numSectors];
			sector_skew.clear();
			for (b = 0;  b < header.numSectors; b++)
			{	
				sector_map[b] = br.read_8();
				headerPtr++;
				if (b == 0) //first sector see if base is 0 or 1 Fluxengine wants 0
					{
//					startSectorId = sector_map[b];		//the start sectorID is for IBM disks always 1. so while we can only write IBM disks with IMD set this default to 1
					startSectorId = 1;		//the start sectorID is for IBM disks is always 1. so while we can only write IBM disks with IMD set this default to 1
					if (sector_map[b]==1)
						{
							blnBaseOne = true;
						}
					}
				if (blnBaseOne==true)
				{
					sector_map[b] = (sector_map[b]-1);
				}
				sector_skew.push_back(sector_map[b] + '0');
			}
			if ((header.Head == 0) and (header.track == 0)) //first read set previous sector skew
			{
				previous_sector_skew = sector_skew;
				previousheader = header;
				previoussectorSize = sectorSize;
			}
			if (!sector_skew.compare(previous_sector_skew) == 0)
			{
				/* Get the previous information and give a warning
				* I had a problem with DD disks written in HP-LIF format and consisting of only 76 tracks. When i read them, the default is 79 tracks so there are 3 tracks in the normal IBM DD format.
				* to make it easier for the user (not having to specify all the tracks he/she wants) i wrote this.
				* the IMD image generated is capable of storing different sector skews per track but fluxengine specifies the sector_skew for IBM disks for the whole image at once
				*/
				std::cout	<< fmt::format("\nWarning as of track {} there is a different sectorskew. New sectorskew {}, old sectorskew {}.\nFluxengine can't write IMD images back to disk with a different sectorskew.\nIgnoring rest of image as of track {}. \n\n",
							header.track, sector_skew, previous_sector_skew, header.track);

				header = previousheader;
				sector_skew = previous_sector_skew;
				sectorSize = previoussectorSize;
				break;
			} else
			{
				//read the sectors
				for (int s = 0; s < header.numSectors; s++)
				{
					Bytes sectordata;
					Bytes compressed(sectorSize);
					std::unique_ptr<Sector>& sector = sectors.get(header.track, header.Head, sector_map[s]);
					sector.reset(new Sector);
					//read the status of the sector
					unsigned int Status_Sector = br.read_8();
					headerPtr++;

					switch (Status_Sector)
					{ 
					/*fluxengine knows of a few sector statussen but not all of the statussen in IMD.
					*  // the statussen are in sector.h. Translation to fluxengine is as follows:
					*	Statussen fluxengine									|	Status IMD		
					*--------------------------------------------------------------------------------------------------------------------
					*  	OK,														|	1, 2 (Normal data: (Sector Size) of (compressed) bytes follow)
					*	BAD_CHECKSUM,											|	
					*	MISSING,		sector not found						|	0 (Sector data unavailable - could not be read)
					*	DATA_MISSING, 	sector present but no data found		|	3, 4, 5, 6, 7, 8
					*	CONFLICT,												|
					*	INTERNAL_ERROR											|
					*/
						case 0: /* Sector data unavailable - could not be read */

							sector->status = Sector::MISSING;
							break;

						case 1: /* Normal data: (Sector Size) bytes follow */
							sectordata = br.read(sectorSize);
							headerPtr += sectorSize;
							sector->data = sectordata;
							sector->status = Sector::OK;
							break;

						case 2: /* Compressed: All bytes in sector have same value (xx) */
							compressed[0] = br.read_8();
							headerPtr++;
							for (int k = 1; k < sectorSize; k++)
							{
								//fill data till sector is full
								br.seek(headerPtr);
								compressed[k] = br.read_8();
							}
							sector->data = compressed;
							sector->status = Sector::OK;
							break;

						case 3: /* Normal data with "Deleted-Data address mark" */
							sector->status = Sector::DATA_MISSING;

							break;

						case 4: /* Compressed with "Deleted-Data address mark"*/
							sector->status = Sector::DATA_MISSING;
						
							break;

						case 5: /* Normal data read with data error - could not be read*/
							sector->status = Sector::DATA_MISSING;
						
							break;

						case 6: /* Compressed read with data error - could not be read */
							sector->status = Sector::DATA_MISSING;

							break;

						case 7: /* Deleted data read with data error - could not be read */
							sector->status = Sector::DATA_MISSING;
						
							break;

						case 8: /* Compressed, Deleted read with data error - could not be read */
							sector->status = Sector::DATA_MISSING;
						
							break;

						default:
							Error() << fmt::format("Don't understand IMD disks with sector status {}", Status_Sector);
					}		
					if (blnOptionalCylinderMap) //there was een optional cylinder map. write is to the sector
					//The Sector Cylinder Map has one entry for each sector, and contains the logical Cylinder ID for the corresponding sector in the Sector Numbering Map.
					{
						sector->physicalTrack = header.track;
						sector->logicalTrack = optionalsector_map[s];
						blnOptionalCylinderMap = false;
					}
					else {

						sector->logicalTrack = sector->physicalTrack = header.track;
					}
					if (blnOptionalHeadMap) //there was een optional head map. write is to the sector
					//The Sector Head Map has one entry for each sector, and contains the logical Head ID for the corresponding sector in the Sector Numbering Map.
					{
						sector->physicalSide = header.Head;
						sector->logicalSide = optionalhead_map[s];
						blnOptionalHeadMap = false;
					}
					else {
						sector->logicalSide = sector->physicalSide = header.Head;
					}
					sector->logicalSector = (sector_map[s]);
					previous_sector_skew = sector_skew;
					previousheader = header;
					sectorSize = previoussectorSize;
				}
			}
  		}
		//Write format detected in IMD image to screen to help user set the right write parameters in case of failures
		size_t headSize = ((header.numSectors) * (sectorSize));
        size_t trackSize = (headSize * (header.Head + 1));

		std::cout 	<< "Reading IMD image\n"
					<< fmt::format("{} tracks, {} heads; {}; {} kbps; {} sectoren; sectorsize {}; sectormap {}; {} kB total \n",
					header.track + 1, header.Head + 1,
					mfm ? "MFM" : "FM",
					Modulation_Speed, header.numSectors, sectorSize, sector_skew, (header.track+1) * trackSize / 1024);
		
		//Set the writer settings based on the read IMD image
		std::string writerinput;
		writerinput= fmt::format(":c={}:h={}:s={}:b={}", header.track + 1, header.Head + 1, header.numSectors, sectorSize);
		setWriterDefaultInput(writerinput);
		std::string writerdest;
		writerdest= fmt::format(":s={}:t=0-{}", header.Head ? "0-1" : "0", header.track);
		setWriterDefaultDest(writerdest);
		
		//Set de (IBM) parameters from within the class IMD read image
		IbmEncoder::setsectorSize(sectorSize);
		IbmEncoder::setstartSectorId(startSectorId);
		IbmEncoder::setuseFm(!mfm);
		IbmEncoder::setclockRateKhz(Modulation_Speed);
		IbmEncoder::setsectorSkew(fmt::format(sector_skew));

		//set the rest with the default (IBM) parameters (because no option was set all these are not filled)
		//First check if these settings have been set bij een flag from the user or we will overwrite them
		if (IbmEncoder::gettrackLengthMs() == 0) IbmEncoder::settrackLengthMs(200);
		if (IbmEncoder::getemitIam() == false) IbmEncoder::setemitIam(true);
		if (IbmEncoder::getidamByte() == 0) IbmEncoder::setidamByte(0x5554);
		if (IbmEncoder::getdamByte() == 0) IbmEncoder::setdamByte(0x5545);
		if (IbmEncoder::getgap0() == 0)	IbmEncoder::setgap0(80);
		if (IbmEncoder::getgap1() == 0)	IbmEncoder::setgap1(50);
		if (IbmEncoder::getgap2() == 0)	IbmEncoder::setgap2(22);
		if (IbmEncoder::getgap3() == 0) IbmEncoder::setgap3(80);
		if (IbmEncoder::getswapSides() == false) IbmEncoder::setswapSides(false);
		return sectors;
 	}
};

std::unique_ptr<ImageReader> ImageReader::createIMDImageReader(
	const ImageSpec& spec)
{
    return std::unique_ptr<ImageReader>(new IMDImageReader(spec));
}


