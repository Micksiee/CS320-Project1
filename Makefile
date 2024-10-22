CC = g++
CFLAGS = -Wall -g
TARGET = predictors

$(TARGET): main.cpp
	$(CC) $(CFLAGS) -o $(TARGET) main.cpp

clean:
	rm -f $(TARGET)
