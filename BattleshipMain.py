import serial
import time
import random

# -----------------------
# Configuration Variables
# -----------------------
# Set this to 1 or 2 to indicate which player this device is.
PLAYER_ID = 1  # Change to 2 for the second device

# Serial port configuration (update these for your system)
SERIAL_PORT = "COM3" if PLAYER_ID == 1 else "COM4"  # example port names; adjust accordingly
BAUD_RATE = 9600

# Board configuration
BOARD_SIZE = 5
SHIP_COUNT = 3

# -----------------------
# Helper Functions
# -----------------------

def generate_board():
    """Creates a BOARD_SIZE x BOARD_SIZE grid with SHIP_COUNT ships placed randomly.
       'O' represents an empty cell, 'S' a ship."""
    board = [['O' for _ in range(BOARD_SIZE)] for _ in range(BOARD_SIZE)]
    placed = 0
    while placed < SHIP_COUNT:
        x, y = random.randint(0, BOARD_SIZE-1), random.randint(0, BOARD_SIZE-1)
        if board[x][y] == 'O':
            board[x][y] = 'S'
            placed += 1
    return board

def print_board(board, cursor=None):
    """Print board to console; if cursor is provided as (x, y), highlight that cell."""
    for i in range(BOARD_SIZE):
        row = ""
        for j in range(BOARD_SIZE):
            cell = board[i][j]
            if cursor is not None and (i, j) == cursor:
                row += f"[{cell}] "  # highlight the cell
            else:
                row += f" {cell}  "
        print(row)
    print()

# These functions simulate reading from directional buttons.
# Replace these with Free-WilI-specific API calls.
def read_button():
    """
    Simulate reading a button press.
    In a real Free-WilI environment, this function would poll hardware input.
    For simulation, type one of: LEFT, RIGHT, UP, DOWN, SELECT (case sensitive).
    """
    btn = input("Press a button (LEFT, RIGHT, UP, DOWN, SELECT): ")
    return btn.strip()

# This function simulates updating the Free-WilI display.
# Replace with actual display API calls.
def update_display(board, cursor):
    print("\n--- Display ---")
    print_board(board, cursor)
    print("---------------")

# -----------------------
# Serial Communication Helpers
# -----------------------
# Opens the serial connection.
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
time.sleep(2)  # Allow connection to settle

def send_message(msg):
    ser.write((msg + "\n").encode())

def read_message():
    line = ser.readline().decode().strip()
    return line if line else None

# -----------------------
# Game Setup
# -----------------------
# Each device keeps its own board (defender board) and an enemy board view.
if PLAYER_ID == 1:
    my_board = generate_board()      # Player 1's board (used when defending)
    enemy_board = [['O' for _ in range(BOARD_SIZE)] for _ in range(BOARD_SIZE)]
else:
    my_board = generate_board()      # Player 2's board (used when defending)
    enemy_board = [['O' for _ in range(BOARD_SIZE)] for _ in range(BOARD_SIZE)]

# For simplicity, we assume the game starts with player 1 attacking.
# A message like "TURN,1" will be sent over serial to sync turns.
# When a turn message is received, both devices update their display to show the defender’s board.
  
# -----------------------
# Game Loop
# -----------------------
def game_loop():
    # Assume an external system or initial broadcast sets the turn.
    # Here, we’ll loop waiting for turn messages and moves.
    game_over = False

    while not game_over:
        msg = None
        # Wait for a TURN message to know whose turn it is.
        while msg is None or not msg.startswith("TURN"):
            msg = read_message()
        _, turn_str = msg.split(",")
        current_turn = int(turn_str)

        # When it's the attacker's turn, both devices display the defender's board.
        if current_turn == PLAYER_ID:
            # I'm the attacker; display the enemy board with a movable cursor.
            cursor_x, cursor_y = 0, 0
            selecting = True
            while selecting:
                update_display(enemy_board, (cursor_x, cursor_y))
                btn = read_button()
                if btn == "LEFT":
                    cursor_y = max(0, cursor_y - 1)
                elif btn == "RIGHT":
                    cursor_y = min(BOARD_SIZE - 1, cursor_y + 1)
                elif btn == "UP":
                    cursor_x = max(0, cursor_x - 1)
                elif btn == "DOWN":
                    cursor_x = min(BOARD_SIZE - 1, cursor_x + 1)
                elif btn == "SELECT":
                    selecting = False
            # Once a coordinate is selected, send the move to the opponent.
            move_msg = f"MOVE,{cursor_x},{cursor_y}"
            send_message(move_msg)
            print(f"Sent move: {cursor_x}, {cursor_y}")
            # Wait for result response.
            result_msg = None
            while result_msg is None or not result_msg.startswith("RESULT"):
                result_msg = read_message()
            _, result = result_msg.split(",")
            # Update the enemy board view accordingly.
            enemy_board[cursor_x][cursor_y] = "X" if result == "HIT" else "M"
            update_display(enemy_board, None)
            print(f"Result of your move: {result}")
            # (You can insert win-check logic here by counting hits on enemy_board.)
            
            # End your turn; next turn goes to the opponent.
            next_turn = 2 if PLAYER_ID == 1 else 1
            send_message(f"TURN,{next_turn}")

        else:
            # I'm the defender; display my board (which is under attack) on both devices.
            update_display(my_board, None)
            print("Waiting for opponent's move...")
            move_msg = None
            while move_msg is None or not move_msg.startswith("MOVE"):
                move_msg = read_message()
            parts = move_msg.split(",")
            move_x, move_y = int(parts[1]), int(parts[2])
            print(f"Opponent attacked: {move_x}, {move_y}")
            # Process the move.
            result = "MISS"
            if my_board[move_x][move_y] == "S":
                result = "HIT"
                my_board[move_x][move_y] = "X"
            else:
                # Mark the miss if the cell was previously empty.
                if my_board[move_x][move_y] == "O":
                    my_board[move_x][move_y] = "M"
            update_display(my_board, None)
            # Send the result back.
            send_message(f"RESULT,{result}")
            print(f"Sent result: {result}")
            # End defender’s turn; now it's my turn as attacker.
            send_message(f"TURN,{PLAYER_ID}")

        # (Optional) Insert game-over condition checks here based on remaining ships.
        time.sleep(1)  # Small pause between turns

# -----------------------
# Start the Game Loop
# -----------------------
game_loop()
