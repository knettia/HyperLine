#define HYPERLINE_FORMAT_IMPLEMENTATION
#include <hyperline/hyperline.hpp> // hyperline
namespace html = hyperline::html;

int main()
{
	html::element test = html::element("html").add_element("head").add_element("body");
}