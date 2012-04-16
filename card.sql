DROP TABLE card;
DROP TABLE category;
DROP TABLE score;

CREATE TABLE card (
    id INTEGER PRIMARY KEY,
    front TEXT,
    back TEXT,
    category INTEGER
);
CREATE TABLE category (
    id INTEGER PRIMARY KEY, 
	name TEXT UNIQUE
);

CREATE TABLE score (
    stamp CURRENT_TIMESTAMP PRIMARY KEY,
    correct INTEGER,
    skipped INTEGER,
    category INTEGER REFERENCES card(id)
);
    

INSERT INTO category VALUES (NULL, "Hebrew");
INSERT INTO category VALUES (NULL, "Spanish");
INSERT INTO category VALUES (NULL, "Hindi");

INSERT INTO card VALUES (NULL, "2x3", "6", 1); 
INSERT INTO card VALUES (NULL, "জঝতঙঌঘড", "dog", 1);
INSERT INTO card VALUES (NULL, "פּטּוּוּ שּׁשׂﬢﬤזּ", "basketball", 1);

INSERT INTO card VALUES (NULL, "Spanish", "blah", 2);
INSERT INTO card VALUES (NULL, "खोजें", "search", 3);
INSERT INTO card VALUES (NULL, "औज़ार-सन्दूक", "in other languages", 3);
INSERT INTO card VALUES (NULL, "से भेजा गया", "current events", 3);
INSERT INTO card VALUES (NULL, "विकिपीडिया", "support", 3);
INSERT INTO card VALUES (NULL, "मुख्य पृष्ठ", "welcome", 3);

/* use datetime ('now') to insert current time */
INSERT INTO score VALUES (datetime('now', '-1 day'), 30, 4, 1);
INSERT INTO score VALUES (datetime('now'), 29, 8, 1);
