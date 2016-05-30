CREATE TABLE IF NOT EXISTS artist 
(artistID INT NOT NULL PRIMARY KEY,
FirstName VARCHAR(30) NOT NULL,
LastName VARCHAR(30) NOT NULL,
Birth DATE,
Death DATE);

CREATE TABLE IF NOT EXISTS painting
(PaintingID INT NOT NULL PRIMARY KEY,
Name VARCHAR(40) NOT NULL,
DateYear INT,
artistID INT NOT NULL FOREIGN KEY REFERENCES artist(artistID));

CREATE TABLE IF NOT EXISTS museum(
museumID INT NOT NULL PRIMARY KEY,
Country VARCHAR(30),
City VARCHAR(30),
Name VARCHAR(50));

CREATE TABLE IF NOT EXISTS exposition
(artistID INT NOT NULL FOREIGN KEY REFERENCES artist(artistID),
museumID INT NOT NULL FOREIGN KEY REFERENCES museum(museumID),
dateBeg DATE,
dateEnd DATE);


INSERT INTO artist (artistID, FirstName, LastName, Birth, Death) VALUES
(1, 'Винсент', 'ван Гог', '1853-03-30', '1890-07-29'),
(2, 'Клод', 'Моне', '1840-11-14', '1926-12-05'),
(3, 'Василий', 'Верещагин', '1842-10-26', '1904-04-13'),
(4, 'Исаак', 'Левитан', '1860-08-30', '1900-08-04'),
(5, 'Михаил', 'Нестеров', '1862-05-31', '1942-10-18');

INSERT INTO painting (paintingID, Name, DateYear, artistID) VALUES
(1,'Звёздная ночь', 1889 , 1),
(2,'Ночная террача кафе', 1888 , 1),
(3,'Пшеничное поле с воронами', 1890, 1),
(4,'Красные виноградники в Арле', 1888, 1),
(5,'Впечатление. Восходящее солнце', 1872, 2),
(6,'Японский мостик', 1900, 2),
(7,'Апофеоз войны', 1871, 3),
(8,'Побежденные. Панихида', 1879, 3),
(9, 'Cкобелев под Шипкой', 1879, 3),
(10,'Осенний день. Сокольники', 1879, 4),
(11,'Вечерний звон', 1892, 4),
(12,'Над вечным покоем', 1893, 4),
(13,'У омута', 1892, 4),
(14,'Лисичка', 1915, 5),
(15,'Философы', 1917, 5),
(16,'Пустынник', 1889, 5);

INSERT INTO museum (museumID, Country, City, Name) VALUES
(1, 'Франция', 'Париж', 'Лувр'),
(2, 'Франция', 'Париж', 'Орсе'),
(3, 'Россия', 'Санкт-Петербург', 'Эрмитаж'),  
(4, 'Россия', 'Москва', 'Третьяковская галерея');

INSERT INTO exposition (artistID, museumID, dateBeg, dateEnd) VALUES
(1, 1, '2014-08-13', '2014-09-13'),  
(1, 2, '2015-09-29', '2015-10-29'),
(2, 1, '2014-06-06', '2014-07-06'),
(2, 3, '2015-06-01', '2015-07-01'),
(3, 3, '2014-11-18', '2014-12-18'),
(3, 4, '2014-12-15', '2015-01-15'), 
(4, 2, '2014-01-18', '2014-02-18'),
(4, 3, '2015-11-04', '2015-11-05'),
(4, 4, '2015-06-07', '2015-07-07'),
(5, 1, '2014-11-29', '2014-12-29'),
(5, 4, '2015-09-16', '2015-10-16');

