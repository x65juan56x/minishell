#include "libft.h"

int	ft_isnumstr(const char *s)
{
    int	i;

    i = 0;
    if (!s || !s[i])
        return (0);
    if (s[i] == '+' || s[i] == '-')
        i++;
    if (!s[i])
        return (0);
    while (s[i])
    {
        if (!ft_isdigit(s[i]))
            return (0);
        i++;
    }
    return (1);
}
