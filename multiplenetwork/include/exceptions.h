#ifndef MULTIPLENETWORK_EXCEPTIONS_H_
#define MULTIPLENETWORK_EXCEPTIONS_H_

#include <exception>
#include <string>

class FileNotFoundException: public std::exception {
public:
	FileNotFoundException(std::string path);
	~FileNotFoundException() throw ();
	virtual const char* what() const throw();
private:
	std::string path;
};

class WrongFormatException: public std::exception {
public:
	WrongFormatException(std::string path);
	~WrongFormatException() throw ();
	virtual const char* what() const throw();
private:
	std::string value;
};


class ElementNotFoundException: public std::exception {
public:
	ElementNotFoundException(std::string value);
	~ElementNotFoundException() throw ();
	virtual const char* what() const throw();
private:
	std::string value;
};

class DuplicateElementException: public std::exception {
public:
	DuplicateElementException(std::string value);
	~DuplicateElementException() throw ();
	virtual const char* what() const throw();
private:
	std::string value;
};

class OperationNotSupportedException: public std::exception {
public:
	OperationNotSupportedException(std::string value);
	~OperationNotSupportedException() throw ();
	virtual const char* what() const throw();
private:
	std::string value;
};

#endif /* MULTIPLENETWORK_EXCEPTIONS_H_ */
