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

	desiredWidth = 150;

	fifoDrawerButton = new FifoDrawerButton("FifoDrawer");
	fifoDrawerButton->setBounds(4, 40, 10, 78);
	fifoDrawerButton->addListener(this);
	addAndMakeVisible(fifoDrawerButton);

	masterLabel = new Label("masterLabel", "MASTER");
	masterLabel->setBounds(7, 21, 40, 20);
	masterLabel->setFont(Font("Small Text", 8.0f, Font::plain));
	addAndMakeVisible(masterLabel);

	masterMonitor = new FifoMonitor(recordNode->recordThread);
	masterMonitor->setBounds(18, 43, 15, 62);
	addAndMakeVisible(masterMonitor);

	masterRecord = new RecordButton("MasterRecord");
	masterRecord->setBounds(18, 110, 15, 15);
	masterRecord->addListener(this);
	addAndMakeVisible(masterRecord);

}

RecordNodeEditor::~RecordNodeEditor()
{

}

void RecordNodeEditor::timerCallback()
{


}

void RecordNodeEditor::updateSubprocessorFifos()
{
	if (recordNode->getNumSubProcessors() != subProcMonitors.size())
	{

		subProcLabels.clear();
		subProcMonitors.clear();
		subProcRecords.clear();

		for (int i = 0; i < recordNode->getNumSubProcessors(); i++)
		{

			subProcLabels.add(new Label("SP"+String(i)));
			subProcLabels.getLast()->setBounds(8 + i *20, 21, 40, 20);
			subProcLabels.getLast()->setFont(Font("Small Text", 8.0f, Font::plain));
			addAndMakeVisible(subProcLabels.getLast());
			subProcLabels.getLast()->setVisible(false);

			subProcMonitors.add(new FifoMonitor(recordNode->recordThread));
			subProcMonitors.getLast()->setBounds(18 + i * 20, 43, 15, 62);
			addAndMakeVisible(subProcMonitors.getLast());
			subProcMonitors.getLast()->setVisible(false);
			
			subProcRecords.add(new RecordButton("RecSP" + String(i)));
			subProcRecords.getLast()->setBounds(18 + i * 20, 110, 15, 15);
			subProcRecords.getLast()->addListener(this);
			addAndMakeVisible(subProcRecords.getLast());
			subProcRecords.getLast()->setVisible(false);
		}
	} 
}

void RecordNodeEditor::buttonClicked(Button *button)
{

	if (button == masterRecord) 
	{

	}
	else if (button == fifoDrawerButton)
	{

		updateSubprocessorFifos();
		if (button->getToggleState())
			showSubprocessorFifos(true);
		else
			showSubprocessorFifos(false);
	}

	buttonEvent(button);
	
}

void RecordNodeEditor::showSubprocessorFifos(bool show)
{

	int dX = 20 * (recordNode->getNumSubProcessors()) + 10;
	if (show)
	{

		fifoDrawerButton->setBounds(
			fifoDrawerButton->getX() + dX, fifoDrawerButton->getY(),
			fifoDrawerButton->getWidth(), fifoDrawerButton->getHeight());

		masterLabel->setBounds(
			masterLabel->getX() + dX, masterLabel->getY(),
			masterLabel->getWidth(), masterLabel->getHeight());

		masterMonitor->setBounds(
			masterMonitor->getX() + dX, masterMonitor->getY(),
			masterMonitor->getWidth(), masterMonitor->getHeight());

		masterRecord->setBounds(
			masterRecord->getX() + dX, masterRecord->getY(),
			masterRecord->getWidth(), masterRecord->getHeight());

		for (auto spl : subProcLabels)
			spl->setVisible(true);
		for (auto spm : subProcMonitors)
			spm->setVisible(true);
		for (auto spr : subProcRecords)
			spr->setVisible(true);

		desiredWidth += dX;
	}
	else
	{

		fifoDrawerButton->setBounds(
			fifoDrawerButton->getX() - dX, fifoDrawerButton->getY(),
			fifoDrawerButton->getWidth(), fifoDrawerButton->getHeight());

		masterLabel->setBounds(
			masterLabel->getX() - dX, masterLabel->getY(),
			masterLabel->getWidth(), masterLabel->getHeight());

		masterMonitor->setBounds(
			masterMonitor->getX() - dX, masterMonitor->getY(),
			masterMonitor->getWidth(), masterMonitor->getHeight());

		masterRecord->setBounds(
			masterRecord->getX() - dX, masterRecord->getY(),
			masterRecord->getWidth(), masterRecord->getHeight());

		for (auto spl : subProcLabels)
			spl->setVisible(false);
		for (auto spm : subProcMonitors)
			spm->setVisible(false);
		for (auto spr : subProcRecords)
			spr->setVisible(false);

		desiredWidth -= dX;
	}
	CoreServices::updateSignalChain(this);
}

FifoDrawerButton::FifoDrawerButton(const String &name) : DrawerButton(name)
{
}

FifoDrawerButton::~FifoDrawerButton()
{
}

void FifoDrawerButton::paintButton(Graphics &g, bool isMouseOver, bool isButtonDown)
{
	if (isMouseOver)
		g.setColour(Colour(210, 210, 210));
	else
		g.setColour(Colour(110, 110, 110));

	g.drawVerticalLine(3, 0.0f, getHeight());
	g.drawVerticalLine(5, 0.0f, getHeight());
	g.drawVerticalLine(7, 0.0f, getHeight());
}

RecordButton::RecordButton(const String& name) : Button(name) {
	setClickingTogglesState(true);
}

RecordButton::~RecordButton() {}

void RecordButton::paintButton(Graphics &g, bool isMouseOver, bool isButtonDown)
{

	g.setColour(Colour(0,0,0));
	g.fillEllipse(0,0,getWidth(),getHeight());

	if (isMouseOver)
	{
		if (getToggleState())
		{
			g.setColour(Colour(255, 65, 65));
		}
		else
		{
			g.setColour(Colour(210, 210, 210));
		}
	}
	else
	{
		if (getToggleState())
		{
			g.setColour(Colour(255, 0, 0));
		}
		else
		{
			g.setColour(Colour(110, 110, 110));
		}
	}
	g.fillEllipse(1,1,getWidth()-2,getHeight()-2);


	/*Draw static black circle in center on top */
	g.setColour(Colour(0,0,0));
	g.fillEllipse(0.35*getWidth(), 0.35*getHeight(), 0.3*getWidth(), 0.3*getHeight());

}


FifoMonitor::FifoMonitor(RecordThread *thread_) : thread(thread_), fillPercentage(0.0)
{
	startTimer(500);
}

void FifoMonitor::timerCallback()
{
	
	if (thread->isThreadRunning())
	{
		setFillPercentage(0.5f);
	}
	else 
	{
		setFillPercentage(random.nextFloat());
	}

	//JFF: Generate a random # between 0-1 and
	


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

	if (fillPercentage < 0.7)	
		g.setColour(Colours::yellow);
	else if (fillPercentage < 0.9)
		g.setColour(Colours::orange);
	else
		g.setColour(Colours::red);
	
	float barHeight = (this->getHeight() - 4) * fillPercentage;
	g.fillRoundedRectangle(2, this->getHeight() - 2 - barHeight, this->getWidth() - 4, barHeight, 2);
}
