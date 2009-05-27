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
 * Revision 1.3  2009-05-27 15:59:37  tino
 * Now works as designed
 *
 * Revision 1.2  2009-05-27 15:44:39  tino
 * Unlinking and options added
 *
 * Revision 1.1  2009-05-27 15:06:29  tino
 * first version
 *
 */

#include "tino/filetool.h"
#include "tino/getopt.h"
#include "tino/num.h"
#include "tino/buf_printf.h"

#include <time.h>

#include "runningfor_version.h"

int
main(int argc, char **argv)
{
  int			argn;
  const char		*tmpdir;
  const char		*file;
  tino_file_stat_t	st;
  unsigned long long	seconds;
  TINO_BUF		buf;
  int			no_ppid, no_unlink;

  argn	= tino_getopt(argc, argv, 1, 2,
		      TINO_GETOPT_VERSION(RUNNINGFOR_VERSION)
		      " seconds [magic]\n"
		      "\tReturns true for the given number of seconds.\n"
		      "\tThe duration is up to 1 second longer due to time rounding.\n"
		      "\tA reference file is created in a temporary directory\n"
		      "\tto measure the time, which is removed on returning false.\n"
		      "\tMagic is in cases when more than one loop is needed\n"
		      "\tas the reference file is named after the parent PID.\n"
		      "\t\n"
		      "\tExample to output dots for 15 seconds without delay:\n"
		      "\twhile runningfor 15; do echo -n .; done"
		      ,

		      TINO_GETOPT_USAGE
		      "h	this help"
		      ,

		      TINO_GETOPT_FLAG
		      "k	Keep reference filename, possibly making it stale.\n"
		      "		Stale reference files always give a false as return code."
		      , &no_unlink,

		      TINO_GETOPT_FLAG
		      "n	do Not include parent pid in reference filename\n"
		      "		If the magic is missing, too, the duration is used instead!"
		      , &no_ppid,

		      TINO_GETOPT_DEFAULT_ENV
		      TINO_GETOPT_STRING
		      TINO_GETOPT_DEFAULT
		      "t dir	Temporary directory for reference file creation"
		      , "RUNNINGFOR_TMPDIR"
		      , &tmpdir,
		      "/tmp",

		      NULL
		      );
  if (argn<=0)
    return 2;

  seconds	= tino_num_secondsA(argv[argn]);

  tino_buf_initO(&buf);
  tino_buf_add_sO(&buf, "runningfor-");
  if (!no_ppid)
    tino_buf_add_sprintfO(&buf, "%ld", (long)getppid());
  if (++argn<argc)
    {
      if (!no_ppid)
	tino_buf_add_cO(&buf, '-');
      tino_buf_add_sO(&buf, argv[argn]);
    }
  else if (no_ppid)
    tino_buf_add_sprintfO(&buf, "%Lu", seconds);
  tino_buf_add_sO(&buf, ".tmp");

  file		= tino_file_glue_pathOi(NULL, 0, tmpdir, tino_buf_get_sN(&buf));

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
  if ((time(NULL)-st.st_ctime)<=seconds)
    return 0;
  if (!no_unlink)
    tino_file_unlinkO(file);
  return 1;
}
