/* Copyright (C) 2007-2008 Chary Corentin.
 *  This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <unistd.h>
#include <stdio.h>

#include "malloc.h"

extern char *__current_brk;
void
chunk_give_back(struct malloc_infos *list)
{
  struct chunk *chunk;

  chunk = list->lmain;
  if (!chunk)
    return;
  if (chunk->size < MALLOC_GIVE_BACK_SIZE)
    return;
  if (chunk->type != MALLOC_CHUNK_FREE)
    return;
#if _MALLOC_USES_SBRK
  if ((char *) chunk + (chunk->size / sizeof(*chunk)) > __curent_brk)
    return;
  chunk_remove(list, chunk);
  brk(chunk);
#endif
}
