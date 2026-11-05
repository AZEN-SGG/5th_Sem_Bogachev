# ifndef io_status_H
# define io_status_H


enum class io_status
{
	success, // not specified
	error_open, // logical and
	error_read, // logical or
};
# endif