#include "SaveManager.h"

using namespace irr;
using namespace core;
using namespace io;

extern IrrlichtDevice* device;

SaveManager::SaveManager()
= default;

bool SaveManager::readHighScore()
{
	if (!device)
		return false;

	//create xml reader
	IXMLReader* xmlReader = device->getFileSystem()->createXMLReader("Save/highScore.xml");
	
	if (!xmlReader)
		return false;
	
	const stringw playerNameTag(L"playerName"); //we'll be looking for this tag in the xml
	stringw currentSection; //keep track of our current section
	const stringw highScoreTag(L"highScore"); //constant for highScoreTag

	//while there is more to read
	while (xmlReader->read())
	{
		//check the node type
		switch (xmlReader->getNodeType())
		{
			//we found a new element
		case EXN_ELEMENT:
		{
			//we currently are in the empty or mygame section and find the highScore tag so we set our current section to highScore
			if (currentSection.empty() && highScoreTag.equals_ignore_case(xmlReader->getNodeName()))
			{
				currentSection = highScoreTag;
			}
			//we are in the highScore section and we find a setting to parse
			else if (currentSection.equals_ignore_case(highScoreTag) && playerNameTag.equals_ignore_case(xmlReader->getNodeName()))
			{
				//read in the key
				stringw key = xmlReader->getAttributeValueSafe(L"name");
				//if there actually is a key to set
				if (!key.empty())
				{
					//set the setting in the map to the value,
					//the [] operator overrides values if they already exist or inserts a new key value
					//pair into the settings map if it was not defined yet
					highScoreMap[key] = xmlReader->getAttributeValueSafe(L"value");
				}
			}
		}
		break;

		//we found the end of an element
		case EXN_ELEMENT_END:
			//we were at the end of the video section so we reset our tag
			currentSection = L"";
			break;
		}
	}

	// don't forget to delete the xml reader
	xmlReader->drop();

	return true;
}

bool SaveManager::writeHighScore() const
{
	//create xml writer
	IXMLWriter* xmlWriter = device->getFileSystem()->createXMLWriter("Save/highScore.xml");

	if (!xmlWriter)
		return false;
	
	//write out the obligatory xml header. Each xml-file needs to have exactly one of those.
	xmlWriter->writeXMLHeader();

	//start element mygame
	xmlWriter->writeElement(L"mygame");
	xmlWriter->writeLineBreak();                  //new line

	//start section with highScore
	xmlWriter->writeElement(L"highScore");
	xmlWriter->writeLineBreak();                  //new line

	// getIterator gets us a pointer to the first node of the settings map
	// every iteration we increase the iterator which gives us the next map node
	// until we reach the end we write scores one by one by using the nodes key and value functions
	map<stringw, stringw>::Iterator i = highScoreMap.getIterator();
	for (; !i.atEnd(); i++)
	{
		//write element as <setting name="key" value="x" />
		//the second parameter indicates this is an empty element with no children, just attributes
		xmlWriter->writeElement(L"playerName", true, L"name", i->getKey().c_str(), L"value", i->getValue().c_str());
		xmlWriter->writeLineBreak();
	}
	xmlWriter->writeLineBreak();

	//close highScore section
	xmlWriter->writeClosingTag(L"highScore");
	xmlWriter->writeLineBreak();

	//close mygame section
	xmlWriter->writeClosingTag(L"mygame");

	//delete xml writer
	xmlWriter->drop();

	return true;
}

void SaveManager::setHighScore(const stringw& name, const stringw& value)
{
	highScoreMap[name] = value;
}

void SaveManager::setHighScore(const stringw& name, s32 value)
{
	highScoreMap[name] = stringw(value);
}