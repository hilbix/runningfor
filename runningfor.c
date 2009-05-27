/* $Header$
 *
 * Return true until the given time periode is reached.
 *
 * Copyright (C)2009 Valentin Hilbig <webmaster@scylla-charybdis.com>
 *
 * This is release early code.  Use at own risk.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 *
 * $Log$
 * Revision 1.1  2009-05-27 15:06:29  tino
 * first version
 *
 */

#include "tino/filetool.h"
#include "tino/getopt.h"
#include "tino/num.h"

#include <time.h>

#include "runningfor_version.h"

int
main(int argc, char **argv)
{
  int			argn;
  const char		*tmpdir;
  char			buf[30];
  const char		*file;
  tino_file_stat_t	st;

  argn	= tino_getopt(argc, argv, 1, 1,
		      TINO_GETOPT_VERSION(RUNNINGFOR_VERSION)
		      " seconds\n"
		      "\tExample to output dots for 15 seconds without delay:\n"
		      "\t# while runningfor 15; do echo -n .; done"
		      ,

		      TINO_GETOPT_USAGE
		      "h	this help"
		      ,

		      TINO_GETOPT_DEFAULT_ENV
		      TINO_GETOPT_STRING
		      TINO_GETOPT_DEFAULT
		      "t dir	Give alternate tmp directory name"
		      , "RUNNINGFOR_TMPDIR"
		      , &tmpdir,
		      "/tmp",

		      NULL
		      );
  if (argn<=0)
    return 2;

  file	= tino_file_glue_pathOi(NULL, 0, tmpdir, tino_snprintf_ret(buf, sizeof buf, "runningfor-%ld.tmp", (long)getppid()));
  while (tino_file_lstatE(file, &st))
    {
      int	fd;

      if ((fd=tino_file_open_createE(file, O_EXCL, 0600))<0)
	{
	  TINO_ERR1("ETTRF103A %s cannot create reference file", file);
	  return 3;
	}
      tino_file_closeE(fd);
    }
  if (st.st_size)
    {
      TINO_ERR1("ETTRF104B %s reference file not empty", file);
      return 4;
    }
  return time(NULL)-st.st_ctime>tino_num_secondsA(argv[argn]);
}
