#define A_WIDTH  10  // Arena width
#define A_HEIGHT 20  // Arena height
#define T_WIDTH  4   // Tetromino width
#define T_HEIGHT 4   // Tetromino height

typedef struct {
    char *buffer;
    size_t buffLen;
    size_t byteLen;
} BufferBuilder;

// Get milliseconds between two clock_t values
clock_t getMs(clock_t start, clock_t end);

// Create a new buffer
BufferBuilder *bbCreate(size_t initialSize);

// Append a string to a buffer
int bbAppend(BufferBuilder *bb, const char *str);

// Free a buffer
void bbFree(BufferBuilder *bb);

// Generate a new tetromino
void newTetromino();

// Determine if a position is valid
bool validPos(int tetromino, int rotation, int posX, int posY);

// Rotate tetromino
int rotate(int x, int y, int rotation);

// Process keyboard inputs
void processInputs();

// Move tetromino down
bool moveDown();

// Add tetromino to arena
void addToArena();

// Check if lines can be cleared
void checkLines();

// Draw arena
void drawArena();