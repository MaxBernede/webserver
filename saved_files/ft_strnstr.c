#include "libft.h"

char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	size_t	i;
	size_t	c;
	char	*ptr;

	i = 0;
	c = 0;
	if (*little == 0)
		return ((char *)big);
	if (len == 0 || *big == 0)
		return ((char *)0);
	while (i < len && big[i])
	{
		while (big[i + c] == little[c] && (i + c) < len && little[c])
			++c;
		ptr = (char *)&big[i];
		if (little[c] == 0)
			return (ptr);
		c = 0;
		++i;
	}
	return (NULL);
}

