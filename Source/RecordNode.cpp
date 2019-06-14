#include "RecordNode.h"

RecordNode::RecordNode() : GenericProcessor("Record Node")
{
	setProcessorType(PROCESSOR_TYPE_FILTER);
}

RecordNode::~RecordNode()
{

}

void RecordNode::setParameter(int parameterIndex, float newValue)
{
	editor->updateParameterButtons(parameterIndex);

	if (currentChannel >= 0)
	{
		Parameter* p = parameters[parameterIndex];
		p->setValue(newValue, currentChannel);
	}
}

void RecordNode::process(AudioSampleBuffer& buffer)
{
	/** 
	If the processor needs to handle events, this method must be called onyle once per process call
	If spike processing is also needing, set the argument to true
	*/
	//checkForEvents(false);
	int numChannels = getNumOutputs();

	for (int chan = 0; chan < numChannels; chan++)
	{
		int numSamples = getNumSamples(chan);
		int64 timestamp = getTimestamp(chan);

		//Do whatever processing needed
		printf("Processing...\n");
	}
	 
}



