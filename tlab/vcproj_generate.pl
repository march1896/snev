#!/usr/local/bin/perl
# The Windows directory structure is immitated with folders in the project so that you can find things easily.
# Required: Windows VS .NET, Perl runtime (such as ActiveState).
#   1. Copy / paste the perl source code below into a text file (like with Notepad) and save it as genvcproj.pl.
#   2. open a command window and chdir to the top level directory of your project (the highest level directory that contains all the source code).
#   3. run the tool with "perl genvcproj.pl myproject"
#   4. the output file, myproject.vcproj, is XML so you can edit it if you want to.
#   5. load the vcproj in MSVS .Net. 


#-----------------------------------------------------------------------
#   vcproj file generator, with recursion
#
#   written by John Henckel, Oct 2004

@d = @ARGV;
$proj = shift @d;

if ($proj eq '-h' || $proj eq '-help' ||
    $proj eq '--help' || $proj eq '-?' ||
    $proj eq '?' || $proj eq '')
{
    print "\nThis creates a dotNET 7.10 vcproj file containing all cpp/hpp ",
          "files in and below specified directories (default current directory). ",
          "All dir are relative to current.\n\n";
    print "Parameter: projectname [dir1 dir2 ... ]\n\n";
    exit;
}

open (FF,">$proj.vcproj") || die "unable to open file $proj.vcproj for output";

print FF <<ZZZ
<?xml version="1.0" encoding="Windows-1252"?>
<VisualStudioProject ProjectType="Visual C++" Version="7.10" Name="$proj" Keyword="MakeFileProj">
<Platforms><Platform Name="Win32"/></Platforms><Configurations>
<Configuration Name="Debug|Win32" OutputDirectory="Debug" IntermediateDirectory="Debug" ConfigurationType="0">
<Tool Name="VCNMakeTool"/></Configuration>
<Configuration Name="Release|Win32" OutputDirectory="Release" IntermediateDirectory="Release" ConfigurationType="0">
<Tool Name="VCNMakeTool"/></Configuration>
</Configurations><References></References>
<Files>
ZZZ
;

push @d,'.' if (!@d);
$i = '';

foreach (@d)
{
    $n = 0;
    print "searching $_ ... ";
    processdir($_,'');
    print "$n files added.\n";
}

# process one directory tree, return true if any source files were found.
sub processdir($)
{
    my $d = shift;
    my $fm = shift;

    opendir(DIR, $d);
    my @list = readdir(DIR);
    closedir DIR;

    $d =~ s/\\/\//g;      # path with '/' separators
    my $name = $d;
    $name =~ s/.*\///;    # just the last part of path
    $name = substr($name,2) if ($name=~/\A\.-/);
    my $path = $d;
    $path =~ s/\//\\/g;   # path with '\' separators
    my $e;
    my $ok = ($name!~/\A\./);   # ignore bad directory names

    $fm .= "$i<Filter Name=\"$name\">\n" if ($ok);

    foreach $e (@list)
    {                                   # source file pattern
        if (-f "$d/$e" && $e=~/(\.c|\.h|cpp|vcc|platform|inc|ph|hpp|kefile|kelist|txt|psh|vu)\Z/i)
        {
            print FF $fm,"$i  <File RelativePath=\"$path\\$e\"></File>\n";
            $fm = '';
            $n++;
        }
        elsif (-d _ && $e!~/\A\./)     # subdir
        {
            $i .="  ";
            $fm = '' if (processdir("$d/$e",$fm));
            $i = substr($i,2);
        }
    }
    print FF "$i</Filter>\n" if ($fm eq '' && $ok);
    return $fm eq '';
}

print FF <<ZZZ
</Files>
<Globals></Globals>
</VisualStudioProject>
ZZZ
;
close FF;
print "done.\n";
exit;

