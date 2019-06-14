//This prevents include loops. We recommend changing the macro to a name suitable for your plugin
#ifndef RECORDNODE_H_DEFINED
#define RECORDNODE_H_DEFINED

#include <ProcessorHeaders.h>

class RecordNode : public GenericProcessor
{
public:
	/** The class constructor, used to initialize any members. */
	RecordNode();

	/** The class destructor, used to deallocate memory */
	~RecordNode();

	/** Defines the functionality of the processor.

	The process method is called every time a new data buffer is available.

	Processors can either use this method to add new data, manipulate existing
	data, or send data to an external target (such as a display or other hardware).

	Continuous signals arrive in the "buffer" variable, event data (such as TTLs
	and spikes) is contained in the "events" variable.
	*/
	void process(AudioSampleBuffer& buffer) override;

	/** Any variables used by the "process" function _must_ be modified only through
	this method while data acquisition is active. If they are modified in any
	other way, the application will crash.  */
	void setParameter(int parameterIndex, float newValue) override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RecordNode);

};

#endif