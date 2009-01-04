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

#ifndef MALLOC_H_
# define MALLOC_H_

# include <stdlib.h>
# include <pthread.h>

# define MALLOC_SPLIT

# ifndef MALLOC_DEBUG
#  define MALLOC_CHUNK_FREE	0
#  define MALLOC_CHUNK_USED	1
# else
#  define MALLOC_CHUNK_FREE	0xdeadbeef
#  define MALLOC_CHUNK_USED	0xbeefdead
# endif

# define MALLOC_MIN_BRK_SIZE	1024
# define MALLOC_GIVE_BACK_SIZE	128

# define MALLOC_SHIFT_LOW	1
# define MALLOC_SHIFT_HIGH	10

# define MALLOC_MINSIZE		(1 << MALLOC_SHIFT_LOW)
# define MALLOC_ALIGNMENT	(sizeof (size_t) * 2)

void *malloc(size_t size);
void *calloc(size_t nmemb, size_t size);

int posix_memalign(void **memptr, size_t alignment, size_t size);

void *valloc(size_t size);
void *memalign(size_t boundary, size_t size);

void *realloc(void *ptr, size_t size);

void free(void *ptr);


/* The size must be a power of two ... */
struct chunk
{
  struct chunk *main_next;
  struct chunk *main_prev;
  struct chunk *free_next;
  struct chunk *free_prev;
  size_t size;
  size_t type;
  size_t asked_size;
  size_t alignment;
};

struct malloc_infos
{
  /* both point to the last member */
  struct chunk *lmain;
  struct chunk *lfree[MALLOC_SHIFT_HIGH + 1];
  pthread_mutex_t mutex;
  int needinit;
};

void free_chunk(void *ptr);
void *get_chunk(size_t size);
void *get_chunk_aligned(size_t size, size_t alignment);
size_t chunk_size(void *ptr);

void chunk_remove_free(struct malloc_infos *list, struct chunk *chunk);
void chunk_append_free(struct malloc_infos *list, struct chunk *chunk);
void chunk_append_main(struct malloc_infos *list, struct chunk *chunk);
void chunk_remove_main(struct malloc_infos *list, struct chunk *chunk);
void chunk_remove(struct malloc_infos *list, struct chunk *chunk);
void chunk_append(struct malloc_infos *list, struct chunk *chunk);
struct chunk *chunk_new(struct malloc_infos *list, size_t units);
struct chunk *chunk_search(struct malloc_infos *list, size_t units);
void chunk_give_back(struct malloc_infos *list);
void chunk_split(struct malloc_infos *list, struct chunk *chunk, size_t size);
void chunk_fusion(struct malloc_infos *list, struct chunk *chunk);

void chunk_insert_free(struct malloc_infos *list, struct chunk *pos,
		       struct chunk *chunk);
void chunk_insert_main(struct malloc_infos *list, struct chunk *pos,
		       struct chunk *chunk);
void show_alloc_mem(void);

# define MIN(a, b)  ((a) < (b) ? (a) : (b))
# define MAX(a, b)  ((a) > (b) ? (a) : (b))

# include <stdio.h>
# include <unistd.h>

static inline int
free_bucket(size_t size)
{
  size_t i;

  i = MALLOC_SHIFT_LOW;
  while (i < MALLOC_SHIFT_HIGH)
    {
      if (size <= (size_t) (1 << i))
	break;
      i++;
    }
  return (i);
}

#endif /* !MALLOC_H_ */
