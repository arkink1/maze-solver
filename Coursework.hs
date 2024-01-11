{- DO NOT CHANGE MODULE NAME, if you do, the file will not load properly -}
module Coursework where

import Data.List
import qualified Data.Set as HS (fromList, toList)
import Test.QuickCheck

{-
  Your task is to design a datatype that represents the mathematical concept of
  a (finite) set of elements (of the same type). We have provided you with an
  interface (do not change this!) but you will need to design the datatype and
  also support the required functions over sets. Any functions you write should
  maintain the following invariant: no duplication of set elements.

  There are lots of different ways to implement a set. The easiest is to use a
  list. Alternatively, one could use an algebraic data type, wrap a binary
  search tree, or even use a self-balancing binary search tree. Extra marks will
  be awarded for efficient implementations (a self-balancing tree will be more
  efficient than a linked list for example).

  You are **NOT** allowed to import anything from the standard library or other
  libraries. Your edit of this file should be completely self-contained.

  **DO NOT** change the type signatures of the functions below: if you do, we
  will not be able to test them and you will get 0% for that part. While sets
  are unordered collections, we have included the Ord constraint on some
  signatures: this is to make testing easier.

  You may write as many auxiliary functions as you need. Everything must be in
  this file.

  See the note **ON MARKING** at the end of the file.
-}

{-
   PART 1.
   You need to define a Set datatype.
-}

-- you **MUST** change this to your own data type. The declaration of Set a =
-- Int is just to allow you to load the file into ghci without an error, it
-- cannot be used to represent a set.
data Color = Red | Black deriving (Eq, Ord, Show)
data Set a = Empty | Node Color (Set a) a (Set a) deriving (Ord)

instance (Show a) => Show (Set a) where
    show s = "{" ++ init (tail (show (toList s))) ++ "}"

{-
   PART 2.
   If you do nothing else, you must get the toList, fromList and equality working. If they
   do not work properly, it is impossible to test your other functions, and you
   will fail the coursework!
-}

-- toList {2,1,4,3} => [1,2,3,4]
-- the output must be sorted.
toList :: Set a -> [a]
toList Empty = []
toList (Node _ c1 x c2) = toList c1 ++ [x] ++ toList c2

-- fromList: do not forget to remove duplicates!
fromList :: Ord a => [a] -> Set a
fromList = foldr Coursework.insert Empty

-- Make sure you satisfy this property. If it fails, then all of the functions
-- on Part 3 will also fail their tests
toFromListProp :: IO ()
toFromListProp =
    quickCheck
        ((\xs -> (HS.toList . HS.fromList $ xs) == (toList . fromList $ xs)) :: [Int] -> Bool)

-- test if two sets have the same elements (pointwise equivalent).
instance (Ord a) => Eq (Set a) where
    s1 == s2 = toList s1 == toList s2

-- you should be able to satisfy this property quite easily
eqProp :: IO ()
eqProp =
  quickCheck ((\xs -> (fromList . Coursework.toList . Coursework.fromList $ xs) == fromList xs) :: [Char] -> Bool)

{-
   PART 3. Your Set should contain the following functions. DO NOT CHANGE THE
   TYPE SIGNATURES.
-}

-- the empty set
empty :: Set a
empty = Empty

-- is it the empty set?
null :: Set a -> Bool
null Empty = True
null _ = False

-- build a one element Set
singleton :: a -> Set a
singleton x = Node Black Empty x Empty

-- insert an element *x* of type *a* into Set *s* make sure there are no
-- duplicates!
insert :: (Ord a) => a -> Set a -> Set a
insert x s = makeBlack (ins s)
    where
        ins Empty = Node Red Empty x Empty
        ins (Node color c1 y c2)
            | x < y = balance color (ins c1) y c2
            | x == y = Node color c1 y c2
            | x > y = balance color c1 y (ins c2)
        makeBlack (Node _ c1 y c2) = Node Black c1 y c2

balance :: Color -> Set a -> a -> Set a -> Set a
balance Black (Node Red (Node Red c1 x c2) y c3) z c4 = Node Red (Node Black c1 x c2) y (Node Black c3 z c4)
balance Black (Node Red c1 x (Node Red c2 y c3)) z c4 = Node Red (Node Black c1 x c2) y (Node Black c3 z c4)
balance Black c1 x (Node Red (Node Red c2 y c3) z c4) = Node Red (Node Black c1 x c2) y (Node Black c3 z c4)
balance Black c1 x (Node Red c2 y (Node Red c3 z c4)) = Node Red (Node Black c1 x c2) y (Node Black c3 z c4)
balance color c1 x c2 = Node color c1 x c2

colorOfNode :: Set a -> Color
colorOfNode Empty = Black
colorOfNode (Node c _ _ _) = c

-- join two Sets together be careful not to introduce duplicates.
union :: (Ord a) => Set a -> Set a -> Set a
union s1 s2 = setfoldr Coursework.insert s2 s1

-- return, as a Set, the common elements between two Sets
intersection :: (Ord a) => Set a -> Set a -> Set a
intersection s1 s2 = setfoldr (\x acc -> if member x s2 then Coursework.insert x acc else acc) s1 Empty

-- all the elements in *s1* not in *s2*
-- {1,2,3,4} `difference` {3,4} => {1,2}
-- {} `difference` {0} => {}
difference :: (Ord a) => Set a -> Set a -> Set a
difference s1 s2 = setfoldr (\x acc -> if member x s2 then acc else Coursework.insert x acc) s1 Empty

-- is element *x* in the Set s1?
member :: (Ord a) => a -> Set a -> Bool
member x Empty = False
member x (Node color c1 y c2)
    | x < y = member x c1
    | x > y = member x c2
    | x == y = True

-- how many elements are there in the Set?
cardinality :: Set a -> Int
cardinality s = setfoldr (const (+1)) s 0

-- apply a function to every element in the Set
setmap :: (Ord b) => (a -> b) -> Set a -> Set b
setmap f Empty = Empty
setmap f (Node color c1 x c2) = Coursework.insert (f x) (Coursework.union (setmap f c1) (setmap f c2))

-- right fold a Set using a function *f*
setfoldr :: (a -> b -> b) -> Set a -> b -> b
setfoldr _ Empty acc = acc
setfoldr f (Node _ c1 x c2) acc = setfoldr f c1 (f x (setfoldr f c2 acc))

-- remove an element *x* from the set
-- return the set unaltered if *x* is not present
removeSet :: (Eq a) => a -> Set a -> Set a
removeSet x Empty = Empty
removeSet x (Node color c1 y c2)
    | x == y = mergeSets c1 c2
    | otherwise = Node color (removeSet x c1) y (removeSet x c2)
        where
            mergeSets :: Set a -> Set a -> Set a
            mergeSets Empty s2 = s2
            mergeSets s1 Empty = s1
            mergeSets s1 s2 = 
                let (minElem, s2') = deleteFindMin s2 
                in balance (colorOfNode s1) s1 minElem s2'

            deleteFindMin :: Set a -> (a, Set a)
            deleteFindMin (Node color c1 x c2)
                | Coursework.null c1 = (x, c2)
                | otherwise =
                    let (minElem, c1') = deleteFindMin c1
                    in (minElem, balance color c1' x c2)

-- powerset of a set
-- powerset {1,2} => { {}, {1}, {2}, {1,2} }
powerSet :: (Ord a) => Set a -> Set (Set a)
powerSet Empty = Coursework.fromList[empty]
powerSet s@(Node _ c1 x c2) = Coursework.union (setmap (Coursework.insert x) (powerSet (Coursework.removeSet x s))) (powerSet (Coursework.removeSet x s))

{-
    To implement the set efficiently, I used red-black binary trees.
    To learn about them I used the following online resources:
    - https://en.wikipedia.org/wiki/Red%E2%80%93black_tree
    - https://www.cs.tufts.edu/comp/150FP/archive/chris-okasaki/redblack99.pdf
-}

{-
   ON MARKING:

   Be careful! This coursework will be marked using QuickCheck, against
   Haskell's own Data.Set implementation. This testing will be conducted
   automatically via a marking script that tests for equivalence between your
   output and Data.Set's output. There is no room for discussion, a failing test
   means that your function does not work properly: you do not know better than
   QuickCheck and Data.Set! Even one failing test means 0 marks for that
   function. Changing the interface by renaming functions, deleting functions,
   or changing the type of a function will cause the script to fail to load in
   the test harness. This requires manual adjustment by a TA: each manual
   adjustment will lose 10% from your score. If you do not want to/cannot
   implement a function, leave it as it is in the file (with undefined).

   Marks will be lost for too much similarity to the Data.Set implementation.

   Pass: creating the Set type and implementing toList and fromList is enough
   for a passing mark of 40%, as long as both toList and fromList satisfy the
   toFromListProp function.

   The maximum mark for those who use Haskell lists to represent a Set is 70%.
   To achieve a higher grade than is, one must write a more efficient
   implementation. 100% is reserved for those brave few who write their own
   self-balancing binary tree.
-}