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

# define MALLOC_SPLIT

# ifndef MALLOC_DEBUG
#  define MALLOC_CHUNK_FREE	0
#  define MALLOC_CHUNK_USED	1
# else
#  define MALLOC_CHUNK_FREE	0x42424242
#  define MALLOC_CHUNK_USED	0x21212121
# endif

# define MALLOC_MIN_BRK_SIZE	1024
# define MALLOC_GIVE_BACK_SIZE	128

# define MALLOC_SHIFT_LOW	1
# define MALLOC_SHIFT_HIGH	10

void	*calloc(size_t nmemb, size_t size);
void	*malloc(size_t size);
void	free(void *ptr);
void	*realloc(void *ptr, size_t size);

struct s_chunk;

/* The size must be a power of two ... */
typedef struct		s_chunk
{
  struct s_chunk	*main_next;
  struct s_chunk	*main_prev;
  struct s_chunk	*free_next;
  struct s_chunk	*free_prev;
  size_t		size;
  size_t		type;
  size_t		asked_size;
  size_t		dummy;
}			t_chunk;

typedef struct		s_chunk_list
{
  /* both point to the last member */
  struct s_chunk	*lmain;
  struct s_chunk	*lfree[MALLOC_SHIFT_HIGH + 1];
  int			lock;
  int			needinit;
}			t_chunk_list;

void	free_chunk(void *ptr);
void	*get_chunk(size_t size);
size_t	chunk_size(void *ptr);

void	chunk_remove_free(t_chunk_list *list, t_chunk *chunk);
void	chunk_append_free(t_chunk_list *list, t_chunk *chunk);
void	chunk_append_main(t_chunk_list *list, t_chunk *chunk);
void	chunk_remove_main(t_chunk_list *list, t_chunk *chunk);
void	chunk_remove(t_chunk_list *list, t_chunk *chunk);
void	chunk_append(t_chunk_list *list, t_chunk *chunk);
t_chunk		*chunk_new(t_chunk_list *list, size_t units);
t_chunk		*chunk_search(t_chunk_list *list, size_t units);
void	chunk_give_back(t_chunk_list *list);
void	chunk_split(t_chunk_list *list, t_chunk *chunk, size_t size);
void	chunk_fusion(t_chunk_list *list, t_chunk *chunk);

void	chunk_insert_free(t_chunk_list *list, t_chunk *pos, t_chunk *chunk);
void	chunk_insert_main(t_chunk_list *list, t_chunk *pos, t_chunk *chunk);

# define MIN(a, b)  ((a) < (b) ? (a) : (b))
# define MAX(a, b)  ((a) > (b) ? (a) : (b))

# include <stdio.h>
# include <unistd.h>

static inline int	free_bucket(size_t size)
{
  size_t		i;

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
