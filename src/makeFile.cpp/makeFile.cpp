#include<string>
#include<iostream>
#include<fstream>
#include <sys/stat.h>
#include <unistd.h>

bool exists (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

void makeFile(std::string const &content, std::string const &location){
	size_t i = 0;
	std::string key = "filename=\"";
	size_t start = content.find(key) + key.length();
	std::string fileName = content.substr(start, content.find('\"', start) - start);
	std::string extension = fileName.substr(fileName.rfind('.'));
	std::string name = fileName.substr(0, fileName.find(extension));
	while (exists(location + fileName) == true){
		fileName = name + std::to_string(i) + extension;
		i++;
	}
	std::string temp = content;
	while (temp[0] != '\n')
		temp.erase(0, (temp.find('\n') + 1));
	temp.erase(0, 1);
	std::ofstream upload(fileName.c_str(), std::ios::out);
	upload << temp;
	upload.close();
}

// #define CONTENT "Content-Disposition: form-data; name=\"i\"; filename=\"main.cpp\"\n\
// Content-Type: text/x-c++src\n\n#include <iostream>\n#include <Array.hpp>\n\n\
// #define MAX_VAL 750\nint main(int, char**)\n{\n    Array<int> numbers(MAX_VAL);\n\
//     int* mirror = new int[MAX_VAL];\n    srand(time(NULL));\n    for (int i = 0; i < MAX_VAL; i++)\n\
//     {\n        const int value = rand();\n        numbers[i] = value;\n        mirror[i] = value;\n\
//     }\n    //SCOPE\n    {\n        Array<int> tmp = numbers;\n        Array<int> test(tmp);\n\
//     }\n\n    for (int i = 0; i < MAX_VAL; i++)\n    {\n        if (mirror[i] != numbers[i])\n\
//         {\n            std::cerr << \"didn't save the same value!!\" << std::endl;\n            return 1;\n\
//         }\n    }\n    try\n    {\n        numbers[-2] = 0;\n    }\n    catch(const std::exception& e)\n\
//     {\n        std::cerr << e.what() << '\\n';\n    }\n    try\n    {\n        numbers[MAX_VAL] = 0;\n\
//     }\n    catch(const std::exception& e)\n    {\n        std::cerr << e.what() << '\\n';\n    }\n\n\
//     for (int i = 0; i < MAX_VAL; i++)\n    {\n        numbers[i] = rand();\n    }\n\
//     delete [] mirror;//\n    return 0;\n}"

// #define PDF "Content-Disposition: form-data; name=\"i\"; filename=\"1193041_Uitnodiging_groepsbezichtiging_20240404125850_10_.pdf\"\n\
// Content-Type: application/pdf\n\n%PDF-1.5\n4 0 obj\n\
// <</Type /Page/Parent 3 0 R/Contents 5 0 R/MediaBox [0 0 595.34997559 842]/Resources<</Font<</FAAAAI 8 0 R/FAAABB 11 0 R/FAAABE 14 0 R>>/XObject<</X1 6 0 R>>>>/Group <</Type/Group/S/Transparency/CS/DeviceRGB>>>>\nendobj\n5 0 obj\n\
// <</Length 18 0 R/Filter /FlateDecode>>stream\nx��Xmo�6�+�1UԻ�aE�f]עoKס�>�6#3�HW�m ?c�xwGI�ci�E7I����=��|�{.�?Y���� ���4�0����?iBc��ȇ\n� ��(��8��A��@/ʳ<��Ov��3�0Z'���n�4�����/��q��?^X���.��t��zA��\"���+x��s��\n\
// �W�ݛx��(\\Ux_�דմ\n*e�:='��8���i�+�����W����c��<3Zww0^�*�j&\n��h	����jC���5��V+�K�)�X��)@z���Eů��L�g�$~�����������/k)�~v�K�4N��U��� � ��c���d�[Z�rS�$^�VO�.���=xE�nU�_�WR�дR�0�\n\
// 檅���o�j(m�m]Iz���*(������lU�xmF�%�[%�Ћ2�$O�{uq�c{�n��u�'H�a\"�4L���!�x�8�5`#em�}�V�Ŝ3��[�)��,��g�+����&H���&��Lf�D��U�<bfX7-�nJ)�Y��;V-\"J=�J��q��蟗%m?-{�#C&0��г�2-��ȅ�Y(�L]��vq��'��\n�u��Z7X�	������\nU\n\
// ��$|i>m@^7K,�E�lM�b�]ƻ���h������nZ&��������E�cy�f&z9���[�"

// int main(){
// 	makeFile(CONTENT, "/home/kposthum/Documents/Make_file.cpp/");
// 	makeFile(PDF, "/home/kposthum/Documents/Make_file.cpp/");
// }