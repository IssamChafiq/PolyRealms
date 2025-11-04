import pandas as pd

# --- load original ---
df = pd.read_excel("WWG Card Galleries - Public.xlsx")

# --- drop columns you said you don't need ---
df = df.drop(columns=['Artist', 'Notes', 'Other'])

def drop_rows_with_word(df, column, word, case=False):
    """
    Deletes rows from a DataFrame where the specified column contains a given word.
    """
    mask = df[column].astype(str).str.contains(word, case=case, na=False)
    return df[~mask]

def drop_rows_with_empty(df, column):
    """
    Deletes rows where the specified column is empty or NaN.
    """
    return df[~df[column].isna() & (df[column].astype(str).str.strip() != "")]

# 1. remove scorecards etc
df = drop_rows_with_word(df, 'Name', 'Scorecard', case=False)

# 2. remove rows with empty Text (we don't want blanks)
df = drop_rows_with_empty(df, 'Text')

# 2.5. remove rows where Text contains the word "token"
df = drop_rows_with_word(df, 'Text', 'token', case=False)
df = drop_rows_with_word(df, 'Text', 'treasure', case=False)

# 3. keep only sets we care about
df = df[df['Set'].isin(['Base Set'])]

# 4. normalize "Type"
def normalize_type(cell):
    """
    Take the raw 'Type' cell and squash it down to just 'Champion', 'Action', or 'Item'.
    """
    if pd.isna(cell):
        return ""

    left = str(cell).split('|')[0].strip()
    first_chunk = left.split('◆')[0].strip()
    first_word = first_chunk.split()[0].strip()

    lowered = first_word.lower()
    if "champion" in lowered:
        return "Champion"
    if "action" in lowered:
        return "Action"
    if "item" in lowered:
        return "Item"

    return first_word

df['Type'] = df['Type'].apply(normalize_type)

# 5. clean Defense
def normalize_defense(cell):
    if pd.isna(cell):
        return ""
    left = str(cell).split('|')[0].strip()
    return left

df['Defense'] = df['Defense'].apply(normalize_defense)

# 6. clean Faction
def normalize_faction(cell):
    if pd.isna(cell):
        return ""
    left = str(cell).split('|')[0].strip()
    return left

df['Faction / Color'] = df['Faction / Color'].apply(normalize_faction)

# 7. add Qty if missing
if 'Qty' not in df.columns:
    df['Qty'] = 1
else:
    df['Qty'] = df['Qty'].fillna(1)
    df['Qty'] = df['Qty'].replace(0, 1)

# 8. reorder columns
desired_cols = [
    'Qty',
    'Name',
    'Text',
    'Type',
    'Faction / Color',
    'Cost',
    'Defense',
    'Role'
]

cols_present = [c for c in desired_cols if c in df.columns]
df = df[cols_present]

unique_values = df['Text'].dropna().astype(str).str.strip().unique().tolist()


# Print the result
print(len(unique_values))

# 9. write final
df.to_csv('BaseSet_Cards_clean.csv', index=False)
