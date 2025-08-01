#include "libft.h"
#include <errno.h>

int	ft_putstrplus_fd(int fd, const char *str)
{
	ssize_t	bytes_written;
	ssize_t	total_written;
	size_t	len;

	if (!str)
		return (0);
	len = ft_strlen(str);
	if (len == 0)
		return (0);
	total_written = 0;
	while ((size_t)total_written < len)
	{
		bytes_written = write(fd, str + total_written, len - total_written);
		if (bytes_written == -1)
		{
			if (errno == EINTR)
				continue ;
			return (-1);
		}
		if (bytes_written == 0)
			return (-1);
		total_written += bytes_written;
	}
	return (0);
}
