/*
 * serializer.h
 *
 *  Created on: 7 апр. 2019 г.
 *      Author: nastya
 */

#ifndef SERIALIZER_H_
#define SERIALIZER_H_

#include <iostream>

using namespace std;

enum class Error
{
    NoError,
    CorruptedArchive
};


class Serializer
{
    static constexpr char Separator = ' ';
    std::ostream& out_;
private:
    template <class T,  class... ArgsT>
	Error process(T&& value, ArgsT&& ... args)
	{
		if (process(std::forward<T>(value)) == Error::NoError)
			return process(std::forward<ArgsT>(args)...);
		else
			return Error::CorruptedArchive;
	}

    Error process(bool value)
    {
    	auto& value_string = out_ << (value ? "true" : "false") << Separator;

    	return value_string ?  Error::NoError : Error::CorruptedArchive;
    }

    Error process(uint64_t value)
    {
    	auto& value_string = out_ << value << Separator;

    	return value_string ? Error::NoError : Error::CorruptedArchive;
    }

    Error process()
    {
    	return Error::NoError;
    }

public:
    explicit Serializer(ostream& out) : out_(out)
    {
    }

    template <class T>
    Error save(T& object)
    {
        return object.serialize(*this);
    }

    template <class... ArgsT>
    Error operator()(ArgsT... args)
    {
        return process(args...);
    }

};



class Deserializer
{
	std::istream& in_;
private:

	 template <class T,  class... ArgsT>
	 Error process(T&& value, ArgsT&& ... args)
	{
		if (process(std::forward<T>(value)) == Error::NoError)
			return process(std::forward<ArgsT>(args)...);
		else
			return Error::CorruptedArchive;
	}

	template <class... ArgsT>
	Error process()
	{
		return Error::CorruptedArchive;
	}

	Error process(bool& value)
	{
	    std::string text;
	    in_ >> text;

	    if (text == "true")
	        value = true;
	    else if (text == "false")
	        value = false;
	    else
	        return Error::CorruptedArchive;

	    return Error::NoError;

	}

	Error process(uint64_t& value)
	{
		std::string value_string;
				in_ >> value_string;

				for (auto symb: value_string)
				{
					if (symb >= '0' && symb <= '9')
						value = value*10  + symb - '0';
					else
						return Error::CorruptedArchive;
				}
				return Error::NoError;
	}
public:

	explicit Deserializer(istream& in) : in_(in)
	{
	}

	template <class T>
	    Error load(T& object)
		{
	        return object.serialize(*this);
	    }

	template <class... ArgsT>
	   Error operator()(ArgsT&&... args)
	   {
	       return process(args...);
	   }
};



#endif /* SERIALIZER_H_ */
