
#include "gen.h"

#include <omnn/math/Variable.h>
#include <omnn/math/VarHost.h>
using namespace omnn::math;

#include <boost/archive/binary_oarchive.hpp>
#include <boost/compute.hpp>
#include <boost/compute/core.hpp>
#include <boost/filesystem.hpp>
#include <boost/gil.hpp>
#include <boost/gil/extension/io/bmp.hpp>
#include <boost/gil/extension/io/targa.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/extension/io/jpeg.hpp>

#include <boost/lambda2.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/program_options.hpp>

#include <string>


namespace {
	std::string StdIn(std::string_view name) {
		std::string str;
		std::cout << ' ' << name << '=';
		do { std::getline(std::cin, str); } while (str.empty());
		return str;
	}
}
int main(int argc, char** argv)
{
	using namespace std::string_view_literals;

    auto& programOptionValues = gen::Init(argc, argv);
	auto filepath = programOptionValues.filepath;
	std::cout << "BG: " << programOptionValues.color << std::endl;

	auto width = 0, height = 0;
	std::cout << "Image size (pixels):" << std::endl
		<< " width="; std::cin >> width;
	std::cout << " height="; std::cin >> height;
	std::cout << std::endl;

	std::cout << "Image pattern (formulas for channels of width, height, x, y):" << std::endl;
	auto names = gen::InitialVarNames();
	auto varhost = names.begin()->second.getVaHost();
	Valuable red(StdIn("Red(w,h,x,y)"), varhost, true);
	Valuable green(StdIn("Green(w,h,x,y)"), varhost, true);
	Valuable blue(StdIn("Blue(w,h,x,y)"), varhost, true);
    std::cout << std::endl;

	auto img = gen::GeneratePatternImage(width, height, red, green, blue);
	auto v = boost::gil::view(img);
    
        std::ofstream file(filepath.c_str(), std::ios_base::out | std::ios_base::binary);
	auto ext = filepath.extension();
	if (ext == ".bmp") {
		boost::gil::write_view(file, v, boost::gil::bmp_tag());
		std::cout << filepath << " written" << std::endl;
	}
	else if (ext == ".png") {
		boost::gil::write_view(file, v, boost::gil::png_tag());
		std::cout << filepath << " written" << std::endl;
	}
	else if (ext == ".jpg" || ext == ".jpeg") {
		boost::gil::rgb8_image_t jpgImg(v.dimensions());
		auto jpgView = view(jpgImg);
		copy_and_convert_pixels(v, jpgView); 
		boost::gil::write_view(file, jpgView, boost::gil::jpeg_tag());
		std::cout << filepath << " written" << std::endl;
	}
	else if (ext != ".tga") {
		std::cout << ext << " extension is not supported, using TGA instead" << std::endl;
		filepath.replace_extension(".tga");
	}
	if(ext == ".tga"){
		boost::gil::write_view(file, v, boost::gil::targa_tag());
		std::cout << filepath << " written" << std::endl;
	}
    file.close();
    return 0;
}
