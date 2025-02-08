#!/usr/bin/perl -w

#
# converts a text file produced by dumphdr.pl into a binary CPX header
#

use strict;
use Data::Dumper;
use IO::Handle;
use Fcntl;

my $me = "genhdr.pl";


sub readcph($)
{
	my ($filename) = @_;
	my $fh;
	my $hdr;
	my $cnt;
	my ($magic, $flags, $id, $version, $text, $icon, $i_color, $title, $t_color);
	my $line;
	$id = 0;
	$flags = 0;
	$version = 0;
	$text = '';
	$icon = '';
	$i_color = 0;
	$title = '';
	$t_color = 0;

	open ($fh, "<", $filename) || die "Can't open $filename: $!";
	while ($line = <$fh>)
	{
		chomp($line);
		if (substr($line, 0, 7) eq 'Flags: ')
		{
			$line = substr($line, 7);
			my @flagstr = split(' ', $line);
			foreach (@flagstr)
			{
				if ($_ eq 'setonly')
				{
					$flags |= 1;
				} elsif ($_ eq 'bootinit')
				{
					$flags |= 2;
				} elsif ($_ eq 'resident')
				{
					$flags |= 4;
				} else
				{
					die "unknown flag $_";
				}
			}
		} elsif (substr($line, 0, 4) eq 'Id: ')
		{
			$id = substr($line, 4);
			if (substr($id, 0, 1) eq "'" && substr($id, -1) eq "'")
			{
				my $c1 = ord(substr($id, 1, 1));
				my $c2 = ord(substr($id, 2, 1));
				my $c3 = ord(substr($id, 3, 1));
				my $c4 = ord(substr($id, 4, 1));
				$id = ($c1 << 24) | ($c2 << 16) | ($c3 << 8) | ($c4 << 0)
			} elsif (substr($id, 0, 2) eq '0x')
			{
				$id = hex($id);
			} else
			{
				die "invalid Id $id";
			}
		} elsif (substr($line, 0, 9) eq 'Version: ')
		{
			$line = substr($line, 9);
			$version = hex($line);
		} elsif (substr($line, 0, 10) eq 'IconText: ')
		{
			$line = substr($line, 10);
			if (substr($line, 0, 1) eq "'" && substr($line, -1) eq "'")
			{
				$text = substr($line, 1, length($line) - 2);
			}
		} elsif (substr($line, 0, 6) eq 'Icon: ')
		{
			$line = substr($line, 6);
			$icon = join('', map(chr, map(hex, split(' ', $line))));
			if (length($icon) != 96)
			{
				die "incorrect icon format (need 96 bytes)"
			}
		} elsif (substr($line, 0, 11) eq 'IconColor: ')
		{
			$line = substr($line, 11);
			$i_color = hex($line);
		} elsif (substr($line, 0, 7) eq 'Title: ')
		{
			$line = substr($line, 7);
			if (substr($line, 0, 1) eq "'" && substr($line, -1) eq "'")
			{
				$title = substr($line, 1, length($line) - 2);
			}
		} elsif (substr($line, 0, 12) eq 'TitleColor: ')
		{
			$line = substr($line, 12);
			$t_color = hex($line);
		} else
		{
			die "unknown keyword $line";
		}
	}
	close($fh);
	if ($id eq '')
	{
		die "missing Id";
	}
	if ($version == 0)
	{
		die "missing Version";
	}
	if ($text eq '')
	{
		die "missing IconText";
	}
	if (length($text) > 13)
	{
		die "IconText too long (max 13 chars)";
	}
	if ($icon eq '')
	{
		die "missing Icon";
	}
	if ($i_color == 0)
	{
		die "missing IconColor";
	}
	if ($title eq '')
	{
		die "missing Title";
	}
	if (length($title) > 17)
	{
		die "Title too long (max 17 chars)";
	}
	if ($t_color == 0)
	{
		die "missing TitleColor";
	}

	$magic = 100;
	$hdr = pack("nnNnZ14a[96]nZ18nZ370", $magic, $flags, $id, $version, $text, $icon, $i_color, $title, $t_color, '');
}

sub usage()
{
	print "$me: generate CPX header from text input\n";
	print "Usage: $me [-o output] <file>\n";
}

#
# main()
#
my $sawfiles = 0;
my $filename;
my $outfile = '';
while ($#ARGV >= 0) {
	if ($ARGV[0] eq '--help') {
		usage();
		exit(0);
	} elsif ($ARGV[0] eq '--version') {
		print "$me version 1.0\n";
		exit(0);
	} elsif ($ARGV[0] eq '-o' || $ARGV[0] eq '--output') {
		shift @ARGV;
		$outfile = $ARGV[0];
		shift @ARGV;
	} elsif ($ARGV[0] =~ /^-/) {
		die "$me: unknown option $ARGV[0]"
	} else {
		if ($sawfiles)
		{
			die "$me: too many arguments";
		}
		$filename = $ARGV[0];
		shift @ARGV;
		$sawfiles = 1;
	}
}	
if (!$sawfiles) {
	print STDERR "$me: missing arguments\n";
	exit(1);
}

my $hdr = readcph($filename);
my $fh;
if ($outfile eq '')
{
	if (-t STDOUT)
	{
		die "won't write binary data to terminal";
	}
	$fh = *STDOUT{IO};
} else
{
	sysopen($fh, $outfile, O_WRONLY|O_CREAT);
}

binmode $fh;


syswrite($fh, $hdr, 512);
