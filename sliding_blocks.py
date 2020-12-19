import math
import copy
import time

class Board:

    solution = []

    def __init__(self, matrix, pos_of_zero, move):
        self.matrix = matrix
        self.position_of_zero = self.__find_position(self.matrix,0)
        self.move = move
        if not self.solution:
            self.__generate_solution(len(matrix),pos_of_zero)
    
    """ 
    Generates all possible board moves from a current state
    """
    def generate_moves(self):
        lst_of_matrices = []
        # move up
        if self.position_of_zero[0] - 1 >= 0:
            lst_of_matrices.append(self.__swap("up",self.position_of_zero[0],self.position_of_zero[1]))
        # move down
        if self.position_of_zero[0] + 1 < len(matrix):
            lst_of_matrices.append(self.__swap("down",self.position_of_zero[0],self.position_of_zero[1]))
        # move left
        if self.position_of_zero[1] - 1 >= 0:
            lst_of_matrices.append(self.__swap("right",self.position_of_zero[0],self.position_of_zero[1]))
        # move right
        if self.position_of_zero[1] + 1 < len(matrix):
            lst_of_matrices.append(self.__swap("left",self.position_of_zero[0],self.position_of_zero[1]))
        return lst_of_matrices

    """ 
    Helper function for swapping the empty element with a nearby element
    """
    def __swap(self, position, row_coord, col_coord):
        new_matrix = copy.deepcopy(self.matrix)
        new_row_coord = 0
        new_col_coord = 0
        if position == "up":
            new_matrix[row_coord][col_coord] = new_matrix[row_coord - 1][col_coord]
            new_matrix[row_coord - 1][col_coord] = 0
            new_row_coord = row_coord - 1
            new_col_coord = col_coord
        elif position == "down":
            new_matrix[row_coord][col_coord] = new_matrix[row_coord + 1][col_coord]
            new_matrix[row_coord + 1][col_coord] = 0
            new_row_coord = row_coord + 1
            new_col_coord = col_coord
        elif position == "right":
            new_matrix[row_coord][col_coord] = new_matrix[row_coord][col_coord - 1]
            new_matrix[row_coord][col_coord - 1] = 0
            new_row_coord = row_coord
            new_col_coord = col_coord - 1
        elif position == "left":
            new_matrix[row_coord][col_coord] = new_matrix[row_coord][col_coord + 1]
            new_matrix[row_coord][col_coord + 1] = 0
            new_row_coord = row_coord
            new_col_coord = col_coord + 1
        else: 
            print("Incorrect usage of swap function")
            return
        return Board(new_matrix,(new_row_coord,new_col_coord), position)

    def __generate_solution(self,n, goal_pos_of_zero):
        lst = list(range(1,n**2))
        if goal_pos_of_zero == -1:
            lst.append(0)
        else:
            # I assume the count of the position starts from 0
            lst.insert(goal_pos_of_zero,goal_pos_of_zero-1)
        i=0
        while i<len(lst):
            self.solution.append(lst[i:i+n])
            i+=n

    def __str__(self):
        return f"{self.matrix}"

    def __find_position(self, matrix, number):
        for index, element in enumerate(matrix):
            if number in element:
                return index, element.index(number)

    """
    Heuristic function
    """
    def manhattan_distance(self):
        manhattan_distance = 0
        for i in range(1,len(self.matrix)**2):
            current_pos_row, current_pos_col = self.__find_position(self.matrix,i)
            ideal_pos_row, ideal_pos_col = self.__find_position(self.solution,i)
            manhattan_distance += abs(current_pos_row - ideal_pos_row) + abs(current_pos_col - ideal_pos_col)
        return manhattan_distance

    def is_solvable(self):
        elements = []
        for row in self.matrix:
            elements.extend(row)
        elements.remove(0)
        sorted_arr, permutations = self.__mergeSortInversions(elements)
        size = len(self.matrix)
        return (size % 2 != 0 and permutations % 2 == 0) or (size % 2 ==0 and (permutations + self.position_of_zero[0])%2!=0)

    """
    Idea: We can use merge sort to count the inversions in a list. This way we reduce the time complexity to O(nlogn)
    """
    def __mergeSortInversions(self,arr):
        if len(arr) <= 1:
            return arr, 0
        else:
            a = arr[:len(arr)//2]
            b = arr[len(arr)//2:]
            a, ai = self.__mergeSortInversions(a)
            b, bi = self.__mergeSortInversions(b)
            c = []
            i = 0
            j = 0
            inversions = 0 + ai + bi
        while i < len(a) and j < len(b):
            if a[i] <= b[j]:
                c.append(a[i])
                i += 1
            else:
                c.append(b[j])
                j += 1
                inversions += (len(a)-i)
        c += a[i:]
        c += b[j:]
        return c, inversions


class Node:
    def __init__(self, parent, board, total_cost,heuristic):
        self.parent = parent
        self.board = board
        self.total_cost = total_cost
        self.heuristic = heuristic
    
    def get_parent(self):
        return self.parent
    
    def is_solution(self):
        return self.board == self.board.solution

class SlidingBlocks:
    def __init__(self, board):
        self.board = board

    def play(self):
        starting_node = Node(parent = None, 
                            board = self.board,
                            total_cost = 0,
                            heuristic = self.board.manhattan_distance())   
        threshold = starting_node.heuristic
        self.ida_star(starting_node)

    def ida_star(self,root):
        bound = root.heuristic
        path = [root]
        while True:
            temp = self.search(path,0,bound)
            if temp == "FOUND":
                return path,bound
            if temp == float("inf"):
                return "NOT FOUND"
            bound = temp
    
    def search(self,path,g,bound):
        current_node = path[-1]
        current_node.total_cost = g + current_node.heuristic
        if current_node.total_cost > bound:
            return current_node.total_cost
        if current_node.board.matrix == current_node.board.solution:
            self.__print_solution(current_node)
            return "FOUND"
        min_val = float("inf")
        for sibling in current_node.board.generate_moves():
            next_node = Node(
                parent=current_node,
                board = sibling,
                total_cost = 0,
                heuristic = sibling.manhattan_distance())
            if next_node not in path:
                path.append(next_node)
            temp = self.search(path,g+1,bound)
            if temp == "FOUND":
                return "FOUND"
            if temp < min_val:
                min_val = temp
            path.pop()
        return min_val

    def __print_solution(self, node):
        steps_list = []
        while node.parent is not None:
            steps_list.append(node.board.move)
            node = node.parent
        print(len(steps_list))
        for i in steps_list[::-1]:
            print(i)

if __name__ == "__main__":
    print("Input number of blocks: ")
    num_of_blocks = int(input())
    block_matrix_dimension = int(math.sqrt(num_of_blocks+1))
    matrix = []
    print("Input position of zero: ")
    pos_of_zero = int(input())
    for j in range(0, block_matrix_dimension):
        row_str = input()
        row = list(map(int, row_str.split(' ')))
        if len(row) > block_matrix_dimension:
            ("wrong input")
            exit(-1)
        matrix.append(row)
    board = Board(matrix,pos_of_zero,"start")
    start_time = time.time()
    if board.is_solvable():
        sliding_blocks = SlidingBlocks(board)
        sliding_blocks.play()
    else:
        print("Board is not solvable!")
    print("--- %s seconds ---" % (time.time() - start_time))