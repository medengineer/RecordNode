/*
------------------------------------------------------------------

This file is part of the Open Ephys GUI
Copyright (C) 2019 Open Ephys

------------------------------------------------------------------

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef __RECORDNODEEDITOR_H__
#define __RECORDNODEEDITOR_H__

#include "RecordChannelSelector.h"

class RecordThread;
class RecordNode;

class FifoDrawerButton : public DrawerButton
{
public:
	FifoDrawerButton(const String& name);
	~FifoDrawerButton();
private:
	void paintButton(Graphics& g, bool isMouseOver, bool isButtonDown) override;
};

class FifoMonitor : public Component, public Timer, public ComponentListener
{
public:
	FifoMonitor(RecordNode*, int);

	void setFillPercentage(float percentage);

	void timerCallback();

	void mouseDoubleClick(const MouseEvent &event);

	void componentBeingDeleted(Component &component);

	std::vector<bool> channelStates;

private :

	OwnedArray<ChannelButton> channelButtons;
	void paint(Graphics &g);

	float fillPercentage;
	RecordNode *recordNode;
	int id;
	Random random;
	
};

class RecordButton : public Button
{
public: 
	RecordButton(const String& name);
	~RecordButton();
private:
	void paintButton(Graphics& g, bool isMouseOver, bool isButtonDown) override;
};

class RecordNodeEditor : public GenericEditor
{
public:

	RecordNodeEditor(RecordNode* parentNode, bool useDefaultParameterEditors);
	virtual ~RecordNodeEditor();

	void collapsedStateChanged() override;

	void updateSubprocessorFifos();
	void showSubprocessorFifos(bool);

	void timerCallback();

private:

	RecordNode* recordNode;

	int numSubprocessors;

	ScopedPointer<FifoDrawerButton> fifoDrawerButton;
	OwnedArray<Label> subProcLabels;
	OwnedArray<FifoMonitor> subProcMonitors;
	OwnedArray<RecordButton> subProcRecords;
	ScopedPointer<Label> masterLabel;
	ScopedPointer<FifoMonitor> masterMonitor;
	ScopedPointer<RecordButton> masterRecord;
	ScopedPointer<Label> engineSelectLabel;
	ScopedPointer<ComboBox> engineSelectCombo;
	ScopedPointer<Label> recordEventsLabel;
	ScopedPointer<RecordButton> eventRecord;
	ScopedPointer<Label> recordSpikesLabel;
	ScopedPointer<RecordButton> spikeRecord;

	bool subprocessorsVisible;

	void buttonEvent(Button* button);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RecordNodeEditor);

};

#endif  // __RECORDNODEEDITOR_H__
