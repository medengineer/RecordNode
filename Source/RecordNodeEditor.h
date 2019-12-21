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
};

class RecordNodeEditor : public GenericEditor
{
public:

	RecordNodeEditor(RecordNode* parentNode, bool useDefaultParameterEditors);
	virtual ~RecordNodeEditor();

	void timerCallback();

private:

	RecordNode* recordNode;

	ScopedPointer<FifoMonitor> firstBuffer;
	ScopedPointer<FifoMonitor> secondBuffer;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RecordNodeEditor);

};

#endif  // __RECORDNODEEDITOR_H__
