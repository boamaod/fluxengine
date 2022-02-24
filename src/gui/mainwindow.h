#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "layout.h"
#include "logger.h"

class CandidateDevice;
class ConfigProto;
class DiskFlux;

class MainWindow : public MainWindowGen
{
public:
    MainWindow();

private:
    void OnExit(wxCommandEvent& event);
	void OnStopButton(wxCommandEvent&);
	void OnReadFluxButton(wxCommandEvent&);
	void OnLogMessage(std::shared_ptr<const AnyLogMessage> message);

	void UpdateVisualisedFlux(std::shared_ptr<const DiskFlux>& flux);
	void UpdateDevices();

private:
	std::vector<std::unique_ptr<ConfigProto>> _formats;
	std::vector<std::unique_ptr<CandidateDevice>> _devices;
	std::shared_ptr<const DiskFlux> _currentDisk;
};

#endif

