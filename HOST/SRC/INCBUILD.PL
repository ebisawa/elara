
	while (<>) {
		if (/^#define ELARA_BUILD		(\d+)$/) {
			print "#define ELARA_BUILD		", $1 + 1, "\n";
		} else {
			print $_;
		}
	}

