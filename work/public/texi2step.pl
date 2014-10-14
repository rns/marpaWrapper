#!perl
# Copyright 2014 Jeffrey Kegler
# This file is part of Libmarpa.  Libmarpa is free software: you can
# redistribute it and/or modify it under the terms of the GNU Lesser
# General Public License as published by the Free Software Foundation,
# either version 3 of the License, or (at your option) any later version.
#
# Libmarpa is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser
# General Public License along with Libmarpa.  If not, see
# http://www.gnu.org/licenses/.

use 5.010;
use strict;
use warnings;
use English qw( -no_match_vars );
use Fatal qw(open close);

if (scalar @ARGV != 1) {
    die("usage: $PROGRAM_NAME step_codes.c > marpa.h-step");
}

open my $codes_c, '>', $ARGV[0];

my @step_type_codes = qw(
MARPA_STEP_INTERNAL1
MARPA_STEP_RULE
MARPA_STEP_TOKEN
MARPA_STEP_NULLING_SYMBOL
MARPA_STEP_TRACE
MARPA_STEP_INACTIVE
MARPA_STEP_INTERNAL2
MARPA_STEP_INITIAL
);

my %step_type_number = map { $step_type_codes[$_], $_ } (0 .. $#step_type_codes);
my @step_types_seen = ();
my @step_types = ();
my $current_step_type_number = undef;

LINE: while ( my $line = <STDIN> ) {

    if ( $line =~ /[@]deftypevr.*MARPA_STEP_/xms ) {
        my ($step_type) = ($line =~ m/(MARPA_STEP_.*)\b/xms);
        if ($step_type) {
            my $step_type_number = $step_type_number{$step_type};
            die("$step_type not in list in $PROGRAM_NAME") if not defined $step_type_number;
            $current_step_type_number = $step_type_number;
            $step_types_seen[$step_type_number] = 1;
            $step_types[$current_step_type_number] = $step_type;
        }
    }

} ## end while ( my $line = <STDIN> )

my $step_type_count = scalar @step_types;
say "#define MARPA_STEP_COUNT $step_type_count";
for ( my $step_type_number = 0; $step_type_number < $step_type_count; $step_type_number++ ) {
    say '#define '
        . $step_types[$step_type_number] . q{ }
        . $step_type_number;
}

say {$codes_c}
    'const struct marpa_step_type_description_s marpa_step_type_description[] = {';
for (
    my $step_type_number = 0;
    $step_type_number < $step_type_count;
    $step_type_number++
    )
{
    my $step_type_name = $step_types[$step_type_number];
    say {$codes_c} qq[  { $step_type_number, "$step_type_name" },];
} ## end for ( my $step_type_number = 0; $step_type_number...)
say {$codes_c} '};';

# vim: expandtab shiftwidth=4:
