# -*- coding: utf-8 -*-

import numpy as np
import pandas as pd
from difflib import SequenceMatcher

def calculate_similarity(a, b):
    return SequenceMatcher(None, a, b).ratio()

def get_scores(search, data):
    scores = np.zeros_like(data)
    for i, j in np.ndindex(data.shape):
        scores[i,j] = calculate_similarity(search, data[i,j])
    return scores 

def main_search():
    df = pd.read_csv('supermarket_aisles.csv')
    all_foods = df.to_numpy()
    # remove leading spaces
    for i, j in np.ndindex(all_foods.shape):
        all_foods[i,j] = all_foods[i,j].lstrip()

    print("Program that searches for an item in the supermarket, and returns the aisle it is in.")

    aisle = None
    search_word = input("Search: ")
    """
    if search_word == "q":
        # quit
        break
    """
    search_scores = get_scores(search_word, all_foods)
    best_score = np.max(search_scores)
    best_idx = np.unravel_index(np.argmax(search_scores), all_foods.shape)
    closest_word = all_foods[best_idx]
    
    aisle_name = df.columns[best_idx[1]]
    aisle_row = best_idx[0] + 1
    aisle_type = all_foods[0,best_idx[1]]

    if best_score < 0.5:
        print("Item not found, please try again.")
    else:
        if best_score != 1:
            print(f"Searching for closest match: {closest_word}")
        if best_idx[0] == 0: # an aisle type rather than an item
            print(f"{aisle_name} is the {aisle_type} aisle.")
        else:
            if closest_word[-1] == 's': # plural
                print(f"{closest_word} are in {aisle_name} ({aisle_type}).")
            else:
                print(f"{closest_word} is in {aisle_name} ({aisle_type}).")

    return aisle_name, aisle_row
    
# Run function
#main_search()