#include <string>
#include <iostream>
#include <tinyxml.h>

void printSchoolXml();
void readSchoolXml();
void writeSchoolXml();
int main(int argc, char** argv) {
	printSchoolXml();
	readSchoolXml();
	writeSchoolXml();
	return 0;
}

void printSchoolXml() {
	using namespace std;
	TiXmlDocument doc;  
	const char * xmlFile = "conf/school.xml";	
	if (doc.LoadFile(xmlFile)) {  	
		doc.Print();  
	} else {
		cout << "can not parse xml conf/school.xml" << endl;
	}
}

void readSchoolXml() {
	using namespace std;
	const char * xmlFile = "conf/school.xml";	
	TiXmlDocument doc;  
	if (doc.LoadFile(xmlFile)) {  	
		doc.Print();  
	} else {
		cout << "can not parse xml conf/school.xml" << endl;
		return;
	}
	TiXmlElement* rootElement = doc.RootElement();  //School元素  
	TiXmlElement* classElement = rootElement->FirstChildElement();  // Class元素
	TiXmlElement* studentElement = classElement->FirstChildElement();  //Students  
	for (; studentElement != NULL; studentElement = studentElement->NextSiblingElement() ) {  
		TiXmlAttribute* attributeOfStudent = studentElement->FirstAttribute();  //获得student的name属性  
		for (;attributeOfStudent != NULL; attributeOfStudent = attributeOfStudent->Next() ) {  
			cout << attributeOfStudent->Name() << " : " << attributeOfStudent->Value() << std::endl;  
		}

		TiXmlElement* studentContactElement = studentElement->FirstChildElement();//获得student的第一个联系方式 
		for (; studentContactElement != NULL; studentContactElement = studentContactElement->NextSiblingElement() ) {
			string contactType = studentContactElement->Value();
			string contactValue = studentContactElement->GetText();
			cout << contactType  << " : " << contactValue << std::endl;  
		}

	}  
}


void writeSchoolXml() {
	using namespace std;
	const char * xmlFile = "conf/school-write.xml";	
	TiXmlDocument doc;  
	TiXmlDeclaration * decl = new TiXmlDeclaration("1.0", "", "");  
	TiXmlElement * schoolElement = new TiXmlElement( "School" );  
	TiXmlElement * classElement = new TiXmlElement( "Class" );  
	classElement->SetAttribute("name", "C++");

	TiXmlElement * stu1Element = new TiXmlElement("Student");
	stu1Element->SetAttribute("name", "tinyxml");
	stu1Element->SetAttribute("number", "123");
	TiXmlElement * stu1EmailElement = new TiXmlElement("email");
	stu1EmailElement->LinkEndChild(new TiXmlText("tinyxml@163.com") );
	TiXmlElement * stu1AddressElement = new TiXmlElement("address");
	stu1AddressElement->LinkEndChild(new TiXmlText("中国"));
	stu1Element->LinkEndChild(stu1EmailElement);
	stu1Element->LinkEndChild(stu1AddressElement);

	TiXmlElement * stu2Element = new TiXmlElement("Student");
	stu2Element->SetAttribute("name", "jsoncpp");
	stu2Element->SetAttribute("number", "456");
	TiXmlElement * stu2EmailElement = new TiXmlElement("email");
	stu2EmailElement->LinkEndChild(new TiXmlText("jsoncpp@163.com"));
	TiXmlElement * stu2AddressElement = new TiXmlElement("address");
	stu2AddressElement->LinkEndChild(new TiXmlText("美国"));
	stu2Element->LinkEndChild(stu2EmailElement);
	stu2Element->LinkEndChild(stu2AddressElement);

	classElement->LinkEndChild(stu1Element);  
	classElement->LinkEndChild(stu2Element);  
	schoolElement->LinkEndChild(classElement);  
	
	doc.LinkEndChild(decl);  
	doc.LinkEndChild(schoolElement); 
	doc.SaveFile(xmlFile);  
}
