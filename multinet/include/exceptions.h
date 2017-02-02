/**
 * exceptions.h
 *
 * This file contains all the exceptions that can be thrown by the functions in this library.
 *
 */

#ifndef MLNET_EXCEPTIONS_H_
#define MLNET_EXCEPTIONS_H_

#include <exception>
#include <string>

namespace mlnet {

/**
 * Exception thrown when a non-existing file is opened.
 */
class FileNotFoundException: public std::exception {
public:
	/**
	 * @param value path of the file
	 */
	FileNotFoundException(std::string value);
	~FileNotFoundException() throw ();
	/**
	 * Information about the exception.
	 * @return an error message describing the occurred problem
	 */
	virtual const char* what() const throw();
private:
	std::string value;
};


/**
 * Exception thrown when a data file is not correctly formatted. See io.h.
 */
class WrongFormatException: public std::exception {
public:
	/**
	 * @param value message explaining what was wrong in the format
	 */
	WrongFormatException(std::string value);
	~WrongFormatException() throw ();
	/**
	 * Information about the exception.
	 * @return an error message describing the occurred problem
	 */
	virtual const char* what() const throw();
private:
	std::string value;
};

/**
 * Exception thrown when an object required by a function does not exist.
 */
class ElementNotFoundException: public std::exception {
public:
	/**
	 * @param value a string indicating type and name (or other identifier) of the element not found
	 */
	ElementNotFoundException(std::string value);
	~ElementNotFoundException() throw ();
	/**
	 * Information about the exception.
	 * @return an error message describing the occurred problem
	 */
	virtual const char* what() const throw();
private:
	std::string value;
};

/**
 * Exception thrown when there is an attempt to create an object that already exists.
 * For example, creating a new layer or a new attribute that are already in the network.
 */
class DuplicateElementException: public std::exception {
public:
	/**
	 * @param value a string indicating type and name (or other identifier) of the element that determined the exception.
	 */
	DuplicateElementException(std::string value);
	~DuplicateElementException() throw ();
	/**
	 * Information about the exception.
	 * @return an error message describing the occurred problem
	 */
	virtual const char* what() const throw();
private:
	std::string value;
};

/**
 * Exception thrown when a function is called with parameters that exist but cannot be handled by the function.
 * For example, generating a random number larger than the maximum possible number,
 * or comparing two paths computed in different networks with different layers.
 */
class OperationNotSupportedException: public std::exception {
public:
	/**
	 * @param value name of the unsupported operation
	 */
	OperationNotSupportedException(std::string value);
	~OperationNotSupportedException() throw ();
	/**
	 * Information about the exception.
	 * @return an error message describing the occurred problem
	 */
	virtual const char* what() const throw();
private:
	std::string value;
};

/**
 * Exception thrown when a function is called with a parameter value that is not
 * among the ones recognized by the function.
 */
class WrongParameterException: public std::exception {
public:
	/**
	 * @param value a string indicating the name and value of the parameter
	 */
	WrongParameterException(std::string value);
	~WrongParameterException() throw ();
	/**
	 * Information about the exception.
	 * @return an error message describing the occurred problem
	 */
	virtual const char* what() const throw();
private:
	std::string value;
};

}

#endif /* MLNET_EXCEPTIONS_H_ */
