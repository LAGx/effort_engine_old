

for i = 1,10 do
   -- Log("hello #"..i , true)
end

h = "hello"


a = Log("hh")
print(a)




--[[


lalka = {
  --  local meme = "A on spit? Hagrit.",
    olou = "none",
    kek = "aa",
    hel = function(l)
        l.olou = "hello"
    end,
    pok = function(l)
        l.olou = "poki"
    end
}

print("hello")
print(lalka.olou)
--[[
arry_lalok[1] = lalka
for i = 2,10 do
    arry_lalok[i] = lalka
end

arry_lalok[5]:hel()
arry_lalok[8]:pok()

for i = 1, 10 do
    print(arry_lalok[i].olou)
end 

print("heh")
]]