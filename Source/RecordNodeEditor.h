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

#include <EditorHeaders.h>

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

class FifoMonitor : public Component, public Timer
{
public:
	FifoMonitor(RecordThread *thread);

	void setFillPercentage(float percentage);

	void timerCallback();

private:
	void paint(Graphics &g);

	float fillPercentage;
	RecordThread *thread;
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

class RecordNodeEditor : public GenericEditor, public Button::Listener
{
public:

	RecordNodeEditor(RecordNode* parentNode, bool useDefaultParameterEditors);
	virtual ~RecordNodeEditor();

	void updateSubprocessorFifos();
	void showSubprocessorFifos(bool);

	void timerCallback();

private:

	RecordNode* recordNode;

	ScopedPointer<FifoDrawerButton> fifoDrawerButton;
	OwnedArray<Label> subProcLabels;
	OwnedArray<FifoMonitor> subProcMonitors;
	OwnedArray<RecordButton> subProcRecords;
	ScopedPointer<Label> masterLabel;
	ScopedPointer<FifoMonitor> masterMonitor;
	ScopedPointer<RecordButton> masterRecord;

	virtual void buttonClicked(Button *button) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RecordNodeEditor);

};

#endif  // __RECORDNODEEDITOR_H__
