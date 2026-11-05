# ifndef Args_H
# define Args_H


class Args
{
	public:
		double *a = nullptr;
		double *b = nullptr;
		double Norma = 0;
		const char *name = nullptr;
		int n = 0;
		int m = 0;
		int p = 0;
		int k = 0;
		int s = 0;
		int r = 0;
		int error = 0;
		double t = 0;
		double t_full = 0;
		double t_2 = 0;
		double t_2_full = 0;
		double r1 = 0;
		double r2 = 0;
		Args () = default;
		Args (const Args &) = delete;
		Args (Args &&) = default;
		Args & operator= (const Args &) = delete;
		Args & operator= (Args &&) = default;
		~Args () = default;
};


# endif