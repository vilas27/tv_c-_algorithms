#include <iostream>
#include <fstream>
#include <sstream>

#include "../rapidxml-1.13/rapidxml.hpp"

int main()
{
	//  Open the xml file
	std::ifstream file("./configs.xml");

	std::stringstream buffer;
	//  Copy the xml file text to a stringstream
	buffer << file.rdbuf();
	file.close();

	//  Convert to string
	std::string content(buffer.str());

	rapidxml::xml_document<> doc;
	//  Parse the xml file text
	doc.parse<0>(&content[0]);

	//  Get the main node
	rapidxml::xml_node<> *root = doc.first_node();
	std::cout << root->name() << std::endl;
	//  Get the main node attributes
	for (rapidxml::xml_attribute<> *attribute = root->first_attribute(); attribute; attribute = attribute->next_attribute())
	{
		std::cout << "\t" << attribute->name() << "\t" << attribute->value() << std::endl;
	}

	//  Get the nodes inside the main node
	for (rapidxml::xml_node<> *node = root->first_node(); node; node = node->next_sibling())
	{
		std::cout << node->name() << std::endl;
		//  Get the nodes attributes
		for (rapidxml::xml_attribute<> *attribute = node->first_attribute(); attribute; attribute = attribute->next_attribute())
		{
			std::cout << "\t" << attribute->name() << "\t" << attribute->value() << std::endl;
		}
	}

	getchar();

	return 0;
}
