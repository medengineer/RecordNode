#include <EditorHeaders.h>

class ChannelButton : public Button	
{
public:
	ChannelButton(int id);
	~ChannelButton();
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

class RecordChannelSelector : public Component
{
public:
	RecordChannelSelector(int nChannels);
	~RecordChannelSelector();
private:
	OwnedArray<ChannelButton> channelButtons;
	OwnedArray<SelectButton> selectButtons;
	ScopedPointer<RangeEditor> rangeEditor;
};
