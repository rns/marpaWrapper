
use ExtUtils::MakeMaker;
WriteMakefile(VERSION        => "7.2.0",
              XS_VERSION     => "7.2.0",
              NAME           => 'libmarpa',
              OBJECT         => 'marpa_ami.o marpa_avl.o marpa.o marpa_codes.o marpa_obs.o marpa_slif.o marpa_tavl.o',
              CCFLAGS        => '',
              LINKTYPE       => 'static');
