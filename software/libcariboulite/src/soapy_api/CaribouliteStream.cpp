#include "Cariboulite.hpp"
#include <Iir.h>
#include <byteswap.h>

#define NUM_BYTES_PER_CPLX_ELEM         ( sizeof(caribou_smi_sample_complex_int16) )

//=================================================================
SoapySDR::Stream::Stream(cariboulite_radio_state_st *radio)
{
    this->radio = radio;
    mtu_size = getMTUSizeElements();
    
    SoapySDR_logf(SOAPY_SDR_INFO, "Creating SampleQueue MTU: %d I/Q samples (%d bytes)", 
				mtu_size, mtu_size * sizeof(caribou_smi_sample_complex_int16));

	// create the actual native queue
	format = CARIBOULITE_FORMAT_INT16;

	// Init the internal IIR filters
    // a buffer for conversion between native and emulated formats
    // the maximal size is the twice the MTU
    interm_native_buffer = new caribou_smi_sample_complex_int16[2 * mtu_size];
	filterType = DigitalFilter_None;
	filter_i = NULL;
	filter_q = NULL;
	filt20_i.setup(4e6, 20e3/2);
	filt50_i.setup(4e6, 50e3/2);
	filt100_i.setup(4e6, 100e3/2);
	filt2p5M_i.setup(4e6, 2.5e6/2);

	filt20_q.setup(4e6, 20e3/2);
	filt50_q.setup(4e6, 50e3/2);
	filt100_q.setup(4e6, 100e3/2);
	filt2p5M_q.setup(4e6, 2.5e6/2);
}

//=================================================================
size_t SoapySDR::Stream::getMTUSizeElements(void)
{
    return cariboulite_get_native_mtu_size_samples(radio);
}

//=================================================================
void SoapySDR::Stream::setDigitalFilter(DigitalFilterType type)
{
	switch (type)
	{
		case DigitalFilter_20KHz: filter_i = &filt20_i; filter_q = &filt20_q; break;
		case DigitalFilter_50KHz: filter_i = &filt50_i; filter_q = &filt50_q; break;
		case DigitalFilter_100KHz: filter_i = &filt100_i; filter_q = &filt100_q; break;
		case DigitalFilter_2500KHz: filter_i = &filt2p5M_i; filter_q = &filt2p5M_q; break;
		case DigitalFilter_None:
		default: 
			filter_i = NULL;
			filter_q = NULL;
			break;
	}
	filterType = type;
}

//=================================================================
SoapySDR::Stream::~Stream()
{
    filterType = DigitalFilter_None;
	filter_i = NULL;
	filter_q = NULL;	
    delete[] interm_native_buffer;
}

//=================================================================
cariboulite_channel_dir_en SoapySDR::Stream::getInnerStreamType(void)
{
	return native_dir;
}

//=================================================================
void SoapySDR::Stream::setInnerStreamType(cariboulite_channel_dir_en direction)
{
    native_dir = direction;
}

//=================================================================
int SoapySDR::Stream::setFormat(const std::string &fmt)
{
	if (!fmt.compare(SOAPY_SDR_CS16))
		format = CARIBOULITE_FORMAT_INT16;
	else if (!fmt.compare(SOAPY_SDR_CS8))
		format = CARIBOULITE_FORMAT_INT8;
	else if (!fmt.compare(SOAPY_SDR_CF32))
		format = CARIBOULITE_FORMAT_FLOAT32;
	else if (!fmt.compare(SOAPY_SDR_CF64))
		format = CARIBOULITE_FORMAT_FLOAT64;
	else
	{
		return -1;
	}
	return 0;
}

//=================================================================
int SoapySDR::Stream::Write(caribou_smi_sample_complex_int16 *buffer, size_t num_samples, uint8_t* meta, long timeout_us)
{
	return cariboulite_radio_write_samples(radio, buffer, num_samples);
}

//=================================================================
int SoapySDR::Stream::Read(caribou_smi_sample_complex_int16 *buffer, size_t num_samples, uint8_t *meta, long timeout_us)
{
    //printf("Reading %d elements\n", num_samples);
    int ret = cariboulite_radio_read_samples(radio, buffer, (caribou_smi_sample_meta*)meta, num_samples);
    if (ret < 0)
    {
        if (ret == -1)
        {
            printf("reader thread failed to read SMI!\n");
        }
        // a special case for debug streams which are not
        // taken care of in the soapy front-end (ret = -2)
        ret = 0;
    }
    //printf("Read %d elements\n", ret);
    return ret;
}

//=================================================================
int SoapySDR::Stream::ReadSamples(caribou_smi_sample_complex_int16* buffer, size_t num_elements, long timeout_us)
{
    int res = Read(buffer, num_elements, NULL, timeout_us);
    if (res < 0)
    {
        SoapySDR_logf(SOAPY_SDR_ERROR, "Reading %d elements failed from queue", num_elements); 
        return res;
    }
    
	if (filterType != DigitalFilter_None && filter_i != NULL && filter_q != NULL)
	{
		for (int i = 0; i < res; i++)
		{
			buffer[i].i = (int16_t)filter_i->filter((float)buffer[i].i);
			buffer[i].q = (int16_t)filter_q->filter((float)buffer[i].q);
		}
	}

    return res;  
}

//=================================================================
int SoapySDR::Stream::ReadSamples(sample_complex_float* buffer, size_t num_elements, long timeout_us)
{
    num_elements = num_elements > mtu_size ? mtu_size : num_elements;

    // read out the native data type
    int res = ReadSamples(interm_native_buffer, num_elements, timeout_us);
    if (res < 0)
    {
        return res;
    }

    float max_val = 4096.0f;

    for (int i = 0; i < res; i++)
    {
        buffer[i].i = (float)(interm_native_buffer[i].i) / max_val;
        buffer[i].q = (float)(interm_native_buffer[i].q) / max_val;
    }
    return res;
}

//=================================================================
int SoapySDR::Stream::ReadSamples(sample_complex_double* buffer, size_t num_elements, long timeout_us)
{
    num_elements = num_elements > mtu_size ? mtu_size : num_elements;

    // read out the native data type
    int res = ReadSamples(interm_native_buffer, num_elements, timeout_us);
    if (res < 0)
    {
        return res;
    }

    double max_val = 4096.0;

    for (int i = 0; i < res; i++)
    {
        buffer[i].i = (double)(interm_native_buffer[i].i) / max_val;
        buffer[i].q = (double)(interm_native_buffer[i].q) / max_val;
    }

    return res;
}

//=================================================================
int SoapySDR::Stream::ReadSamples(sample_complex_int8* buffer, size_t num_elements, long timeout_us)
{
    num_elements = num_elements > mtu_size ? mtu_size : num_elements;

    // read out the native data type
    int res = ReadSamples(interm_native_buffer, num_elements, timeout_us);
    if (res < 0)
    {
        return res;
    }

    for (int i = 0; i < res; i++)
    {
        buffer[i].i = (int8_t)((interm_native_buffer[i].i >> 5)&0x00FF);
        buffer[i].q = (int8_t)((interm_native_buffer[i].q >> 5)&0x00FF);
    }

    return res;
}

//=================================================================
int SoapySDR::Stream::ReadSamplesGen(void* buffer, size_t num_elements, long timeout_us)
{
	switch (format)
	{
		case CARIBOULITE_FORMAT_FLOAT32: return ReadSamples((sample_complex_float*)buffer, num_elements, timeout_us); break;
	    case CARIBOULITE_FORMAT_INT16: return ReadSamples((caribou_smi_sample_complex_int16*)buffer, num_elements, timeout_us); break;
	    case CARIBOULITE_FORMAT_INT8: return ReadSamples((sample_complex_int8*)buffer, num_elements, timeout_us); break;
	    case CARIBOULITE_FORMAT_FLOAT64: return ReadSamples((sample_complex_double*)buffer, num_elements, timeout_us); break;
		default: return ReadSamples((caribou_smi_sample_complex_int16*)buffer, num_elements, timeout_us); break;
	}
	return 0;
}