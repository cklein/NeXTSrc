char *test_vals[] = {
	"1",
	"1.0",
	"-1",
	"-1.0",
	"1e2",
	"1.0e2",
	"-1e2",
	"-1.0e2",
	"1e-2",
	"1.0e-2",
	"-1e-2",
	"-1.0e-2",
	"1e100",
	"1.0e100",
	"-1e100",
	"-1.0e100",
	"1e-100",
	"1.0e-100",
	"-1e-100",
	"-1.0e-100",
	"12345678",
	"-12345678",
	"12345678e10",
	"-12345678e10",
	"12345678e-10",
	"-12345678e-10",
	"1234.5678",
	"-1234.5678",
	"1234.5678e10",
	"-1234.5678e10",
	"1234.5678e-10",
	"-1234.5678e-10",
	"12345678901234567890",
	"12345678901234567890e10",
	"12345678901234567890e100",
	"12345678901234567890e300",
	"-12345678901234567890",
	"-12345678901234567890e10",
	"-12345678901234567890e100",
	"-12345678901234567890e300",
	"12345678901234567890",
	"12345678901234567890e-10",
	"12345678901234567890e-100",
	"12345678901234567890e-300",
	"-12345678901234567890",
	"-12345678901234567890e-10",
	"-12345678901234567890e-100",
	"-12345678901234567890e-300",
	".12345678901234567890",
	".12345678901234567890e10",
	".12345678901234567890e100",
	".12345678901234567890e300",
	"-.12345678901234567890",
	"-.12345678901234567890e10",
	"-.12345678901234567890e100",
	"-.12345678901234567890e300",
	".12345678901234567890",
	".12345678901234567890e-10",
	".12345678901234567890e-100",
	".12345678901234567890e-300",
	"-.12345678901234567890",
	"-.12345678901234567890e-10",
	"-.12345678901234567890e-100",
	"-.12345678901234567890e-300",
	"1234567890.1234567890",
	"1234567890.1234567890e10",
	"1234567890.1234567890e100",
	"1234567890.1234567890e300",
	"-1234567890.1234567890",
	"-1234567890.1234567890e10",
	"-1234567890.1234567890e100",
	"-1234567890.1234567890e300",
	"1234567890.1234567890",
	"1234567890.1234567890e-10",
	"1234567890.1234567890e-100",
	"1234567890.1234567890e-300",
	"-1234567890.1234567890",
	"-1234567890.1234567890e-10",
	"-1234567890.1234567890e-100",
	"-1234567890.1234567890e-300",
	0
};

main()
{
	double atof();
	register char **cp;

	for (cp = test_vals; *cp; cp++)
		printf("s=%s, atof=%.20g\n", *cp, atof(*cp));
	exit(0);
}
