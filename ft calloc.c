#include "libft.h"

void	*ft_calloc(size_t nmemb, size_t size)
{
	char	*ptr;
	size_t	c;
	size_t	added;

	added = size * nmemb;
	ptr = malloc(added);
	if (!ptr)
		return (NULL);
	c = 0;
	while (c < added)
	{
		ptr[c] = '\0';
		++c;
	}
	return ((void *)ptr);
}

