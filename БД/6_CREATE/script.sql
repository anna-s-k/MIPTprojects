CREATE TABLE IF NOT EXISTS artist 
(artistID INT NOT NULL,
FirstName VARCHAR(30) NOT NULL,
LastName VARCHAR(30) NOT NULL,
Birth DATE,
Death DATE);

CREATE TABLE IF NOT EXISTS painting
(PaintingID INT NOT NULL,
Name VARCHAR(40) NOT NULL,
DateYear INT,
artistID VARCHAR(30));

CREATE TABLE IF NOT EXISTS museum(
museumID INT,
Country VARCHAR(30),
City VARCHAR(30),
Name VARCHAR(50));

CREATE TABLE IF NOT EXISTS exposition
(artistID VARCHAR(30) NOT NULL,
museumID INT,
dateBeg DATE,
dateEnd DATE);


INSERT INTO artist (artistID, FirstName, LastName, Birth, Death) VALUES
(1, 'Винсент', 'ван Гог', '30-03-1853', '29-07-1890'),
(2, 'Клод', 'Моне', '14-11-1840', '05-12-1926'),
(3, 'Василий', 'Верещагин', '26-10-1842', '13-04-1904'),
(4, 'Исаак', 'Левитан', '30-08-1860', '04-08-1900'),
(5, 'Михаил', 'Нестеров', '31-05-1862', '18-10-1942');

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
(1, 1, 
(1, 2
(2, 1
(2, 3
(3, 3
(3, 4
(4, 2
(4, 3
(4, 4
(5, 1
(5, 4
