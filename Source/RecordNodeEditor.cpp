/*
------------------------------------------------------------------

This file is part of the Open Ephys GUI
Copyright (C) 2014 Open Ephys

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

#include <stdio.h>
#include "RecordNodeEditor.h"
#include "RecordNode.h"

RecordNodeEditor::RecordNodeEditor(RecordNode* parentNode, bool useDefaultParameterEditors = true)
	: GenericEditor(parentNode, useDefaultParameterEditors)

{

	recordNode = parentNode;

	desiredWidth = 120;

	firstBuffer = new FifoMonitor(recordNode->recordThread);
	firstBuffer->setBounds(10,40,15,75);
	addAndMakeVisible(firstBuffer);

	secondBuffer = new FifoMonitor(recordNode->recordThread);
	secondBuffer->setBounds(30, 40, 15, 75);
	addAndMakeVisible(secondBuffer);

	//startTimer(500);


}

RecordNodeEditor::~RecordNodeEditor()
{

}

void RecordNodeEditor::timerCallback()
{


}

FifoMonitor::FifoMonitor(RecordThread *thread_) : thread(thread_), fillPercentage(0.0)
{
	startTimer(1000); // update fill percentage every 0.5 seconds
}

void FifoMonitor::timerCallback()
{
	//TODO
	if (thread->isThreadRunning())
	{
		std::cout << "First buffer: " << thread->recordNode->samplesWritten << std::endl;
		std::cout << "Second buffer: " << thread->samplesWritten << std::endl;
		setFillPercentage(0.5f);
	}
	else {
		setFillPercentage(0.25f);
	}


}

void FifoMonitor::setFillPercentage(float fill_)
{
	fillPercentage = fill_;

	repaint();
}

void FifoMonitor::paint(Graphics &g)
{
	g.setColour(Colours::grey);
	g.fillRoundedRectangle(0, 0, this->getWidth(), this->getHeight(), 4);
	g.setColour(Colours::lightslategrey);
	g.fillRoundedRectangle(2, 2, this->getWidth() - 4, this->getHeight() - 4, 2);

	g.setColour(Colours::yellow);
	float barHeight = (this->getHeight() - 4) * fillPercentage;
	g.fillRoundedRectangle(2, this->getHeight() - 2 - barHeight, this->getWidth() - 4, barHeight, 2);
}
