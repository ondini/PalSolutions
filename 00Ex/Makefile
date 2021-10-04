CFLAGS+= -Wall -g
CC = g++
ZIP = zip

TG = fence
CF = main.cpp

$(TG): $(CF)
	$(CC) $(CFLAGS) $< -o $@

zip:
	$(ZIP) $(TG).zip $(CF)

clean:
	$(RM) *.o
	$(RM) $(TG)
	$(RM) $(TG).zip

.PHONY: clean zip
