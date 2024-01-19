#pragma once

#ifdef __cplusplus

#include <google/protobuf/message.h>
#include "lib/config.pb.h"
#include "lib/common.pb.h"

class ConfigProto;
class OptionProto;
class FluxSource;
class FluxSink;
class ImageReader;
class ImageWriter;
class Encoder;
class Decoder;

class OptionException : public ErrorException
{
public:
    OptionException(const std::string& message): ErrorException(message) {}
};

class OptionNotFoundException : public OptionException
{
public:
    OptionNotFoundException(const std::string& message):
        OptionException(message)
    {
    }
};

class InvalidOptionException : public OptionException
{
public:
    InvalidOptionException(const std::string& message): OptionException(message)
    {
    }
};

class InapplicableOptionException : public OptionException
{
public:
    InapplicableOptionException(const std::string& message):
        OptionException(message)
    {
    }
};

class InapplicableValueException : public ErrorException
{
public:
    InapplicableValueException():
        ErrorException("selected format cannot be used here")
    {
    }
};

struct FluxConstructor
{
    std::string name;
    std::regex pattern;
    std::string glob;
    std::function<void(const std::string& filename, FluxSourceProto*)> source;
    std::function<void(const std::string& filename, FluxSinkProto*)> sink;
};

class Config
{
public:
    /* Direct access to the various proto layers. */

    const ConfigProto* operator->()
    {
        return combined();
    }

    operator const ConfigProto&()
    {
        return *combined();
    }

    ConfigProto* base()
    {
        invalidate();
        return &_baseConfig;
    }

    ConfigProto* overrides()
    {
        invalidate();
        return &_overridesConfig;
    }

    ConfigProto* combined();

    /* Force the combined config to be rebuilt. */

    void invalidate();

    /* Check option validity. Returns a list of errors. */

    std::vector<std::string> validate();
    void validateAndThrow();

    /* Set and get individual config keys on the override config. */

    void set(std::string key, std::string value);
    std::string get(std::string key);

    /* Set a config key on the combined config. This will disappear the next
     * time the config is rebuilt. */

    void setTransient(std::string key, std::string value);

    /* Reset the entire configuration. */

    void clear();

    /* Merge in one config file into the base config. */

    void readBaseConfigFile(std::string filename);
    void readBaseConfig(std::string data);

    /* Option management: look up an option by name, determine whether an option
     * is valid, and apply an option. */

    const OptionProto& findOption(const std::string& option);
    void checkOptionValid(const OptionProto& option);
    bool isOptionValid(const OptionProto& option);
    bool isOptionValid(std::string option);
    void applyOption(const OptionProto& option);
    void applyOption(std::string option);
    void clearOptions();

    /* Adjust overall inputs and outputs. */

    void setFluxSource(std::string value);
    void setFluxSink(std::string value);
    void setVerificationFluxSource(std::string value);
    void setCopyFluxTo(std::string value);
    void setImageReader(std::string value);
    void setImageWriter(std::string value);

    /* Fetch the sources, opening them if necessary. */

    bool hasFluxSource();
    std::shared_ptr<FluxSource>& getFluxSource();
    bool hasImageReader();
    std::shared_ptr<ImageReader>& getImageReader();
    bool hasVerificationFluxSource() const;
    std::shared_ptr<FluxSource>& getVerificationFluxSource();

    /* Fetch the encoder and decoder, creating them if necessary. */

    bool hasEncoder();
    std::shared_ptr<Encoder>& getEncoder();
    bool hasDecoder();
    std::shared_ptr<Decoder>& getDecoder();

    /* Create the sinks: these are not cached. */

    bool hasFluxSink();
    std::unique_ptr<FluxSink> getFluxSink();
    bool hasImageWriter();
    std::unique_ptr<ImageWriter> getImageWriter();

public:
    static const std::vector<FluxConstructor>& getFluxFormats();
    static std::vector<std::string> getImageFormats();

private:
    ConfigProto _baseConfig;
    ConfigProto _overridesConfig;
    ConfigProto _combinedConfig;
    std::set<std::string> _appliedOptions;
    bool _configValid;

    std::shared_ptr<FluxSource> _fluxSource;
    std::shared_ptr<ImageReader> _imageReader;
    std::shared_ptr<FluxSource> _verificationFluxSource;
    std::shared_ptr<Encoder> _encoder;
    std::shared_ptr<Decoder> _decoder;
    FluxSourceProto _verificationFluxSourceProto;
};

extern Config& globalConfig();

#endif
