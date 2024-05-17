#include "libft.h"

static int	check(char const *s1, char const *set, int i)
{
	int	c;

	while (s1[i])
	{
		c = 0;
		while (set[c])
		{
			if (s1[i] == set[c])
				break ;
			++c;
		}
		if (s1[i] == set[c])
			++i;
		else
			break ;
	}
	return (i);
}

static int	checkend(char const *s1, char const *set, int i)
{
	int	c;

	while (s1 != 0)
	{
		c = 0;
		while (set[c])
		{
			if (s1[i] == set[c])
				break ;
			++c;
		}
		if (s1[i] == set[c])
			--i;
		else
			break ;
	}
	return (i + 1);
}

char	*ft_strtrim(char const *s1, char const *set)
{
	char	*ptr;
	int		start;
	int		end;

	if (!s1)
		return (NULL);
	if (!set)
		return (ft_strdup(s1));
	start = check(s1, set, 0);
	end = checkend(s1, set, ft_strlen(s1));
	ptr = ft_substr(s1, start, end - start);
	if (!ptr)
		return (NULL);
	return (ptr);
}

