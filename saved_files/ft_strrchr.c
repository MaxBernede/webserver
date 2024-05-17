#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	char	*ptr;
	char	*temp;

	ptr = (char *)s;
	temp = 0;
	while (*ptr)
	{
		if (*ptr == (char)c)
			temp = ptr;
		ptr++;
	}
	if (*ptr == (char)c)
		return (ptr);
	return (temp);
}

