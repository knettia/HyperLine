#pragma once

// std
#include <string>
#include <string_view>

#include <vector>
#include <variant>
#include <optional>

#include <algorithm>
#include <functional>

// hyperline
#include "../__format.hpp" // __hyperline_format
#include "../__template.hpp" // __hyperline_convertible_template
#include "../__funcexcept.hpp" // __throw_runtime_error

#include "./__attribute.hpp"

namespace hyperline::html::__1
{
	inline static const std::vector<std::string> __closing_tags
	{ "br", "img", "input", "meta", "link", "hr", "area", "base", "col", "command", "keygen", "source", "track", "wbr" };

	inline constexpr bool __is_closing_tag(const std::string &tag)
	{ return std::find(__closing_tags.begin(), __closing_tags.end(), tag) != __closing_tags.end(); }

	struct element
	{
		using attribute_vector = std::vector<attribute>;

		using inner_content = std::variant<std::string, element>;
		using inner_content_vector = std::vector<inner_content>;

	private:
		std::string __tag;
		attribute_vector __attributes;

		inner_content_vector __inner_content;
		
		inline static void __validate_tag(const std::string &tag)
		{
			if (tag.empty() || std::any_of(tag.begin(), tag.end(), ::isspace))
			{ __throw_runtime_error(__hyperline_format("invalid element tag \"{0}\"; XML tags may not be empty or contain whitespaces", tag)); }
		}

		inline static std::string represent(const element &e)
		{
			std::string attr_str;
			for (const attribute &attr : e.__attributes)
			attr_str += __hyperline_format(" {0}=\"{1}\"", attr.key(), attr.value());

			std::string start_tag = __hyperline_format("<{0}>", e.__tag + attr_str);
			if (e.__inner_content.empty())
			{
				if (__is_closing_tag(e.__tag)) { return start_tag; }
				else { return start_tag.substr(0, start_tag.size() - 1) + " />"; }
			}
			
			std::string content;
			for (const inner_content &item : e.__inner_content)
			{
				if (std::holds_alternative<std::string>(item)) { content += std::get<std::string>(item); }
				else if (std::holds_alternative<element>(item)) { content += element::represent(std::get<element>(item)); }
			}

			std::string result = start_tag + content + __hyperline_format("</{0}>", e.__tag);
			return result;
		}

		inline static std::vector<attribute>::const_iterator __find_attribute(const std::vector<attribute> &attributes, const std::string &k)
		{
			std::vector<attribute>::const_iterator iter = std::find_if(attributes.begin(), attributes.end(), 
				[&k](const attribute& attr)
				{ return attr.key() == k; }
			);

			return iter;
		}
	public:
		template <typename __string_type, __hyperline_convertible_template(__string_type, std::string_view)>
		inline element(__string_type tag)
		{
			std::string t(tag);

			__validate_tag(t);
			__tag = std::move(t);
		}

		inline std::string get_tag() const
		{ return __tag; }

		template <typename __string_type, __hyperline_convertible_template(__string_type, std::string_view)>
		inline bool has_attribute(const __string_type &k) const
		{
			auto iter = __find_attribute(__attributes, k);

			if (iter != __attributes.end())
			{ return true; }
			
			return false;
		}

		template <typename __string_type, __hyperline_convertible_template(__string_type, std::string_view)>
		inline attribute &get_attribute(const __string_type &k) const
		{
			auto iter = __find_attribute(__attributes, k);

			if (iter != __attributes.end())
			{ return iter->value; }
			
			__throw_runtime_error(__hyperline_format("could not find attribute with key \"{0}\" in HTML element <{1}>", k, __tag));
		}

		template <typename __string_type, __hyperline_convertible_template(__string_type, std::string_view)>
		inline std::optional<attribute> get_attribute_optional(const __string_type &k)
		{
			auto iter = __find_attribute(__attributes, k);

			if (iter != __attributes.end())
			{ return iter->value; }
			
			return std::nullopt;
		}

		template <typename __string_type, __hyperline_convertible_template(__string_type, std::string_view)>
		inline element &delete_attribute(const __string_type &k)
		{
			auto iter = __find_attribute(__attributes, k);

			if (iter != __attributes.end())
			{
				__attributes.erase(iter);
				return *this;
			}

			__throw_runtime_error(__hyperline_format("could not delete attribute with key \"{0}\" in HTML element <{1}>; no attribute found", k, __tag));
		}

		template <
			typename __string_type_k,
			typename __string_type_v,
			__hyperline_convertible_template(__string_type_k, std::string_view),
			__hyperline_convertible_template(__string_type_v, std::string_view)
		>
		inline element &append_attribute(const __string_type_k &k, const __string_type_v &v = "")
		{
			__attributes.emplace_back(std::string(k), std::string(v));
			return *this;
		}

		inline element &append_attribute(const attribute &x)
		{
			__attributes.emplace_back(x);
			return *this;
		}

		// template <
		// 	typename __string_type_k,
		// 	typename __string_type_v,
		// 	__hyperline_convertible_template(__string_type_k, std::string_view),
		// 	__hyperline_convertible_template(__string_type_v, std::string_view)
		// >
		// inline element &prepend_attribute(const __string_type_k &k, const __string_type_v &v = "")
		// {
		// 	__attributes.emplace(__attribites.begin(), std::string(k), std::string(v));
		// 	return *this;
		// }

		// inline element &prepend_attribute(const attribute &x)
		// {
		// 	__attributes.emplace(__attributes.begin(), x);
		// 	return *this;
		// }

		inline element &push_back(const element &x)
		{
			if (__is_closing_tag(__tag)) { __throw_runtime_error(__hyperline_format("HTML tag \"{0}\" cannot contain inner content", __tag)); }
			__inner_content.emplace_back(x);
			return *this;
		}

		template <typename __string_type, __hyperline_convertible_template(__string_type, std::string_view)>
		inline element &push_back(const __string_type &s)
		{
			if (__is_closing_tag(__tag)) { __throw_runtime_error(__hyperline_format("HTML tag \"{0}\" cannot contain inner content", __tag)); }

			__inner_content.push_back(std::string(s));
			return *this;
		}

		template <typename __string_type, __hyperline_convertible_template(__string_type, std::string_view)>
		inline element &push_back(const __string_type &s, const bool parse)
		{
			if (__is_closing_tag(__tag)) { __throw_runtime_error(__hyperline_format("HTML tag \"{0}\" cannot contain inner content", __tag)); }

			if (parse)
			{
				std::vector<std::variant<std::string, element>> content = element::parse(s);
				for (std::variant<std::string, element> ic : content)
				{
					if (std::holds_alternative<std::string>(ic))
					{ this->push_back(std::get<std::string>(ic)); }
					else if (std::holds_alternative<element>(ic))
					{ this->push_back(std::get<element>(ic)); }
				}
			}
			else
			{ __inner_content.push_back(std::string(s)); }

			return *this;
		}

		template <typename __string_type, __hyperline_convertible_template(__string_type, std::string_view)>
		inline element &add_element(
			__string_type tag,
			const std::function<void(element&)> &callback = nullptr
		)
		{
			element child(tag);
			if (callback)
			{ callback(child); }
			
			this->push_back(child);
			return *this;
		}

		// shortcut
		inline element &add_break(const std::function<void(element&)> &callback = nullptr)
		{
			this->add_element("br", callback);
			return *this;
		}

		operator std::string() const
		{
			return represent(*this);
		}

		friend std::ostream& operator<<(std::ostream &os, const element &elem)
		{
			os << represent(elem);
			return os;
		}

		static std::vector<std::variant<std::string, element>> parse(const std::string &content)
		{
			std::vector<std::variant<std::string, element>> result;
			std::vector<std::string> tag_stack;
			size_t i = 0;

			while (i < content.size())
			{
				while (i < content.size() && std::isspace(content[i])) ++i;

				if (i < content.size() && content[i] == '<')
				{
					size_t tag_start = i;

					size_t tag_end = content.find('>', tag_start);
					if (tag_end == std::string::npos)
					{ __throw_runtime_error("malformed tag; no closing '>' found"); }

					std::string tag_content = content.substr(tag_start + 1, tag_end - tag_start - 1);

					if (tag_content[0] == '/')
					{
						std::string closing_tag = tag_content.substr(1);
						if (tag_stack.empty() || tag_stack.back() != closing_tag)
						{ __throw_runtime_error(__hyperline_format("unexpected or mismatched closing tag </{0}>", closing_tag)); }

						tag_stack.pop_back();
						i = tag_end + 1;
						break;
					}

					bool is_self_closing = tag_content.back() == '/';
					if (is_self_closing)
					{
						tag_content.pop_back();
					}

					size_t space_pos = tag_content.find(' ');
					std::string tag_name = tag_content.substr(0, space_pos);
					element elem(tag_name);

					if (space_pos != std::string::npos)
					{
						std::string attributes_str = tag_content.substr(space_pos + 1);
						size_t attr_pos = 0;

						while (attr_pos < attributes_str.size())
						{
							size_t key_end = attributes_str.find('=', attr_pos);
							if (key_end == std::string::npos)
							{ break; }

							std::string key = attributes_str.substr(attr_pos, key_end - attr_pos);
							key.erase(std::remove_if(key.begin(), key.end(), ::isspace), key.end());

							size_t value_start = attributes_str.find('"', key_end + 1);
							size_t value_end = attributes_str.find('"', value_start + 1);
							if (value_start == std::string::npos || value_end == std::string::npos)
							{ break; }

							std::string value = attributes_str.substr(value_start + 1, value_end - value_start - 1);

							elem.append_attribute(key, value);

							attr_pos = value_end + 1;
						}
					}

					i = tag_end + 1;

					if (!is_self_closing)
					{
						tag_stack.push_back(tag_name);

						std::vector<std::variant<std::string, element>> inner_content = parse(content.substr(i));

						for (auto &child : inner_content)
						{
							if (std::holds_alternative<std::string>(child))
							{ elem.push_back(std::get<std::string>(child)); }
							else if (std::holds_alternative<element>(child))
							{ elem.push_back(std::get<element>(child)); }
						}

						size_t closing_tag_start = content.find(__hyperline_format("</{0}>", tag_name), i);
						if (closing_tag_start == std::string::npos)
						{ __throw_runtime_error(__hyperline_format("malformed HTML: no closing tag after <{0}>", tag_name)); }

						i = closing_tag_start + tag_name.size() + 3;
						tag_stack.pop_back();
					}

					result.emplace_back(std::move(elem));
				}
				else
				{
					size_t text_end = content.find('<', i);
					if (text_end == std::string::npos)
					{
						result.emplace_back(content.substr(i));
						break;
					}
					else
					{
						result.emplace_back(content.substr(i, text_end - i));
						i = text_end;
					}
				}
			}

			if (!tag_stack.empty())
			{ __throw_runtime_error(__hyperline_format("unmatched opening tag <{0}>", tag_stack.back())); }

			return result;
		}

	};
}