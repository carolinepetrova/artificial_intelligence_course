# function which generates the board of frogs and the solution
def generate_board(num_of_frogs):
    board = "_"
    solution = "_"
    for i in range(num_of_frogs):
        board = "L" + board + "R"
        solution = "R" + solution + "L"
    return board,solution


def swap(c, i, j):
    new = list(c)
    new[i], new[j] = new[j], new[i]
    return ''.join(new)

# generates all possible moves of the frogs on a given board
def move_generator(current_board):
    all_moves = []
    # we find where the blank tile
    find_blank_tile = current_board.find('_')
    # we determine if there is a frog from the left which is 1 or 2 places behind the blank tile
    if find_blank_tile > 0 and current_board[find_blank_tile-1] == 'L':
            all_moves.append(swap(current_board,find_blank_tile-1,find_blank_tile))
    if find_blank_tile > 2 and current_board[find_blank_tile-2] == 'L':
            all_moves.append(swap(current_board,find_blank_tile-2,find_blank_tile))
    # same idea for the frogs from the right
    if find_blank_tile < len(current_board)-1 and current_board[find_blank_tile+1] == 'R':
            all_moves.append(swap(current_board,find_blank_tile+1,find_blank_tile))
    if find_blank_tile < len(current_board)-2 and current_board[find_blank_tile+2] == 'R':
            all_moves.append(swap(current_board,find_blank_tile+2,find_blank_tile))
    return all_moves

def solve(board, solution):
    state_graph = { }
    # the stack consists of a vertex of the graph and the path from the start to it
    # this way we obtain the path of right moves to solve the frog game
    stack = [(board, [board])]
    visited = set()
    # The idea here is that the DFS algorithm searches for the solution and in the same time
    # builds the state graph. Тhis way we save some time instead of first generating the state graph and then traversing it. 
    while stack:
        (vertex, path) = stack.pop()
        if vertex not in visited:
            if vertex == solution:
                 return path
            visited.add(vertex)
            # if the vertex is not visited that means we need to generate all the possible moves
            # from it
            state_graph[vertex] = move_generator(vertex)
            for neighbor in state_graph[vertex]:
                stack.append((neighbor, path + [neighbor]))

if __name__ == "__main__":
    print("Input number of frogs: ")
    num_of_frogs = int(input()) 
    board, solution = generate_board(num_of_frogs)
    solution_path = solve(board,solution)
    for i in solution_path:
        print(i)
