#include <EditorHeaders.h>

enum Select { ALL, NONE, RANGE };

class ChannelButton : public Button	
{
public:
	ChannelButton(int id);
	~ChannelButton();
    int getId() { return id; };
private:
	int id; 
    int width;
    int height;
	void paintButton(Graphics& g, bool isMouseOver, bool isButtonDown) override;
};

class SelectButton : public Button	
{
public:
	SelectButton(const String& name);
	~SelectButton();
private:
	void paintButton(Graphics& g, bool isMouseOver, bool isButtonDown) override;
};

class RangeEditor : public TextEditor
{
public:
	RangeEditor(const String& name);
	~RangeEditor();
private:
	//TODO:
};

class RecordChannelSelector : public Component, public Button::Listener, public TextEditor::Listener
{
public:
	RecordChannelSelector(int nChannels);
	~RecordChannelSelector();
private:
	int convertStringToInteger(String s);
	Array<int> parseStringIntoRange(int rangeValue);
	void buttonClicked(Button *);
	void textEditorReturnKeyPressed(TextEditor &);
	void updateRangeString();
	void parseRangeString();
	OwnedArray<ChannelButton> channelButtons;
	OwnedArray<SelectButton> selectButtons;
	ScopedPointer<RangeEditor> rangeEditor;

	int nChannels;
	String rangeString;
	Array<int> channelStates;
    
};
